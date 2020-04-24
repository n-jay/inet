//
// Copyright (C) 2013 OpenSim Ltd
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
// author: Zoltan Bojthe
//

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/lifecycle/OperationalBase.h"

namespace inet {

OperationalBase0::OperationalBase0(cSimpleModule& module)
  : module(module)
{
}

OperationalBase0::~OperationalBase0()
{
    module.cancelAndDelete(activeOperationExtraTimer);
    module.cancelAndDelete(activeOperationTimeout);
}

void OperationalBase0::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        WATCH(operationalState);
        activeOperationTimeout = new cMessage("ActiveOperationTimeout");
        activeOperationExtraTimer = new cMessage("ActiveOperationExtraTimer");
        activeOperationExtraTimer->setSchedulingPriority(std::numeric_limits<short>::max());
    }
    if (isInitializeStage(stage)) {
        auto state = getInitialOperationalState();
        ASSERT(state == NOT_OPERATING || state == OPERATING);
        setOperationalState(state);
        if (operationalState == OPERATING)
            handleStartOperation(nullptr);     //TODO use an InitializeOperation with current stage value
    }
}

void OperationalBase0::handleMessage(cMessage *message)
{
    if (message == activeOperationExtraTimer)
        finishActiveOperation();
    else if (message == activeOperationTimeout) {
        module.cancelEvent(activeOperationExtraTimer);
        handleActiveOperationTimeout(message);
    }
    else {
        switch (operationalState) {
            case STARTING_OPERATION:
            case OPERATING:
            case STOPPING_OPERATION:
                handleMessageWhenUp(message);
                break;
            case NOT_OPERATING:
                handleMessageWhenDown(message);
                break;
            case CRASHING_OPERATION:
            default:
                throw cRuntimeError("Invalid operational state: %d", (int)operationalState);
        }
    }
}

void OperationalBase0::handleMessageWhenDown(cMessage *message)
{
    if (message->isSelfMessage())
        throw cRuntimeError(&module, "Self message '%s' received when %s is down", message->getName(), module.getComponentType()->getName());
    else if (simTime() == lastChange)
        EV_WARN << module.getComponentType()->getName() << " is down, dropping '" << message->getName() << "' message\n";
    else
        throw cRuntimeError(&module, "Message '%s' received when %s is down", message->getName(), module.getComponentType()->getName());
    delete message;
}

bool OperationalBase0::handleOperationStage(LifecycleOperation *operation, IDoneCallback *doneCallback)
{
//    Enter_Method_Silent();
    omnetpp::cMethodCallContextSwitcher __ctx(&module); __ctx.methodCallSilent();

    int stage = operation->getCurrentStage();
    if (dynamic_cast<ModuleStartOperation *>(operation)) {
        if (isModuleStartStage(stage)) {
            operationalState = STARTING_OPERATION;
            setupActiveOperation(operation, doneCallback, OPERATING);
            handleStartOperation(operation);
            if (activeOperation.operation != nullptr && !activeOperation.isDelayedFinish)
                finishActiveOperation();
            return activeOperation.operation == nullptr;
        }
    }
    else if (dynamic_cast<ModuleStopOperation *>(operation)) {
        if (isModuleStopStage(stage)) {
            operationalState = STOPPING_OPERATION;
            setupActiveOperation(operation, doneCallback, NOT_OPERATING);
            handleStopOperation(operation);
            if (activeOperation.operation != nullptr && !activeOperation.isDelayedFinish)
                finishActiveOperation();
            return activeOperation.operation == nullptr;
        }
    }
    else if (dynamic_cast<ModuleCrashOperation *>(operation)) {
        if (stage == ModuleCrashOperation::STAGE_CRASH) {
            operationalState = CRASHING_OPERATION;
            setupActiveOperation(operation, doneCallback, NOT_OPERATING);
            handleCrashOperation(operation);
            finishActiveOperation();
            return true;
        }
    }
    else
        throw cRuntimeError("unaccepted Lifecycle operation: (%s)%s", operation->getClassName(), operation->getName());
    return true;
}

void OperationalBase0::scheduleOperationTimeout(simtime_t timeout)
{
    ASSERT(activeOperation.isDelayedFinish);
    ASSERT(!activeOperationTimeout->isScheduled());
    activeOperationTimeout->setContextPointer(activeOperation.operation);
    module.scheduleAt(simTime() + timeout, activeOperationTimeout);
    // TODO: schedule timer and use module parameter
}

void OperationalBase0::handleActiveOperationTimeout(cMessage *message)
{
    handleCrashOperation(activeOperation.operation);
    finishActiveOperation();
}

void OperationalBase0::setupActiveOperation(LifecycleOperation *operation, IDoneCallback *doneCallback, State endState)
{
    ASSERT(activeOperation.operation == nullptr);
    activeOperation.set(operation, doneCallback, endState);
}

void OperationalBase0::setOperationalState(State newState)
{
    operationalState = newState;
    lastChange = simTime();
}

OperationalBase0::State OperationalBase0::getInitialOperationalState() const
{
    cModule *node = findContainingNode(&module);
    NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
    return (!nodeStatus || nodeStatus->getState() == NodeStatus::UP) ? OPERATING : NOT_OPERATING;
}

void OperationalBase0::delayActiveOperationFinish(simtime_t timeout)
{
    ASSERT(activeOperation.operation != nullptr);
    activeOperation.delayFinish();
    scheduleOperationTimeout(timeout);
}

void OperationalBase0::startActiveOperationExtraTime(simtime_t extraTime)
{
    ASSERT(extraTime >= SIMTIME_ZERO);
    ASSERT(!activeOperationExtraTimer->isScheduled());
    activeOperation.delayFinish();
    setOperationalState(activeOperation.endState);
    module.scheduleAt(simTime() + extraTime, activeOperationExtraTimer);
}

void OperationalBase0::startActiveOperationExtraTimeOrFinish(simtime_t extraTime)
{
    if (extraTime >= SIMTIME_ZERO)
        startActiveOperationExtraTime(extraTime);
    else
        finishActiveOperation();
}

void OperationalBase0::finishActiveOperation()
{
    setOperationalState(activeOperation.endState);
    if (activeOperation.isDelayedFinish) {
        module.cancelEvent(activeOperationExtraTimer);
        module.cancelEvent(activeOperationTimeout);
        activeOperation.doneCallback->invoke();
    }
    activeOperation.clear();
}

void OperationalBase0::refreshDisplay() const
{
    switch (operationalState) {
    case STARTING_OPERATION:
        module.getDisplayString().setTagArg("i2", 0, "status/up");
        break;
    case OPERATING:
        module.getDisplayString().removeTag("i2");
        break;
    case STOPPING_OPERATION:
    case CRASHING_OPERATION:
        module.getDisplayString().setTagArg("i2", 0, "status/down");
        break;
    case NOT_OPERATING:
        module.getDisplayString().setTagArg("i2", 0, "status/cross");
        break;
    default:
        break;
    }
}

} // namespace inet

