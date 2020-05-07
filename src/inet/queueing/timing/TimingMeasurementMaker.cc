//
// Copyright (C) OpenSim Ltd.
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
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "inet/common/FlowTagSet_m.h"
#include "inet/queueing/timing/TimingMeasurementMaker.h"

namespace inet {
namespace queueing {

simsignal_t TimingMeasurementMaker::lifeTimeSignal = cComponent::registerSignal("lifeTime");
simsignal_t TimingMeasurementMaker::elapsedTimeSignal = cComponent::registerSignal("elapsedTime");
simsignal_t TimingMeasurementMaker::delayingTimeSignal = cComponent::registerSignal("delayingTime");
simsignal_t TimingMeasurementMaker::queueingTimeSignal = cComponent::registerSignal("queueingTime");
simsignal_t TimingMeasurementMaker::processingTimeSignal = cComponent::registerSignal("processingTime");
simsignal_t TimingMeasurementMaker::transmissionTimeSignal = cComponent::registerSignal("transmissionTime");
simsignal_t TimingMeasurementMaker::propagationTimeSignal = cComponent::registerSignal("propagationTime");

Define_Module(TimingMeasurementMaker);

static bool matchesString(cMatchExpression& matchExpression, const char *string)
{
    cMatchableString matchableString(string);
    return matchExpression.matches(&matchableString);
}

void TimingMeasurementMaker::initialize(int stage)
{
    PacketFlowBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        offset = b(par("offset"));
        length = b(par("length"));
        finish = par("finish");
        label = par("label");
        labelMatcher.setPattern(label, false, true, true);
        cMatchExpression measureMatcher;
        measureMatcher.setPattern(par("measure"), false, true, true);
        measureLifeTime = matchesString(measureMatcher, "lifeTime");
        measureElapsedTime = matchesString(measureMatcher, "elapsedTime");
        measureDelayingTime = matchesString(measureMatcher, "delayingTime");
        measureQueueingTime = matchesString(measureMatcher, "queueingTime");
        measureProcessingTime = matchesString(measureMatcher, "processingTime");
        measureTransmissionTime = matchesString(measureMatcher, "transmissionTime");
        measurePropagationTime = matchesString(measureMatcher, "propagationTime");
    }
}

void TimingMeasurementMaker::processPacket(Packet *packet)
{
    const auto& data = packet->peekData();
    if (*label == '\0') {
        if (measureLifeTime)
            data->mapAllTags<CreationTimeTag>(offset, length, [&] (b offset, b length, const CreationTimeTag *timeTag) {
                makeMeasurement(packet, offset, length, nullptr, lifeTimeSignal, simTime() - timeTag->getCreationTime());
            });
        if (measureElapsedTime)
            data->mapAllTags<ElapsedTimeTag>(offset, length, [&] (b offset, b length, const ElapsedTimeTag *timeTag) {
                makeMeasurement(packet, offset, length, nullptr, elapsedTimeSignal, simTime() - timeTag->getStartTime());
            });
        data->mapAllTags<TimeTagBase>(offset, length, [&] (b offset, b length, const TimeTagBase *timeTag) {
            makeMeasurement(packet, offset, length, nullptr, timeTag);
        });
    }
    else {
        data->mapAllTags<FlowTagSet>(offset, length, [&] (b offset, b length, const FlowTagSet *flowTagSet) {
            for (int i = 0; i < (int)flowTagSet->getLabelsArraySize(); i++) {
                auto label = flowTagSet->getLabels(i);
                auto tag = flowTagSet->getTags(i);
                cMatchableString matchableLabel(label);
                if (labelMatcher.matches(&matchableLabel)) {
                    cNamedObject signalDetails(label);
                    if (auto timeTag = dynamic_cast<const CreationTimeTag *>(tag)) {
                        if (measureLifeTime)
                            makeMeasurement(packet, offset, length, label, lifeTimeSignal, simTime() - timeTag->getCreationTime());
                    }
                    else if (auto timeTag = dynamic_cast<const ElapsedTimeTag *>(tag)) {
                        if (measureElapsedTime)
                            makeMeasurement(packet, offset, length, label, elapsedTimeSignal, simTime() - timeTag->getStartTime());
                    }
                    else if (auto timeTag = dynamic_cast<const TimeTagBase *>(tag))
                        makeMeasurement(packet, offset, length, label, timeTag);
                    else
                        throw cRuntimeError("Unknown tag");
                }
            }
        });
    }
}

void TimingMeasurementMaker::makeMeasurement(Packet *packet, b offset, b length, const char *label, simsignal_t signal, simtime_t value)
{
    EV_INFO << "Making measurement on packet " << packet->getName() << ": "
            << "range (" << offset << ", " << offset + length << "), ";
    if (label != nullptr)
        EV_INFO << "label = " << label << ", ";
    EV_INFO << "signal = " << cComponent::getSignalName(signal) << ", ";
    EV_INFO << "value = " << value << std::endl;
    cNamedObject signalDetails(label);
    emit(signal, value, &signalDetails);
}

void TimingMeasurementMaker::makeMeasurement(Packet *packet, b offset, b length, const char *label, const TimeTagBase *timeTag)
{
    if (dynamic_cast<const DelayingTimeTag *>(timeTag)) {
        if (measureDelayingTime)
            return makeMeasurement(packet, offset, length, label, delayingTimeSignal, timeTag->getTotalTime());
    }
    else if (dynamic_cast<const QueueingTimeTag *>(timeTag)) {
        if (measureQueueingTime)
            return makeMeasurement(packet, offset, length, label, queueingTimeSignal, timeTag->getTotalTime());
    }
    else if (dynamic_cast<const ProcessingTimeTag *>(timeTag)) {
        if (measureProcessingTime)
            return makeMeasurement(packet, offset, length, label, processingTimeSignal, timeTag->getTotalTime());
    }
    else if (dynamic_cast<const TransmissionTimeTag *>(timeTag)) {
        if (measureTransmissionTime)
            return makeMeasurement(packet, offset, length, label, transmissionTimeSignal, timeTag->getTotalTime());
    }
    else if (dynamic_cast<const PropagationTimeTag *>(timeTag)) {
        if (measurePropagationTime)
            return makeMeasurement(packet, offset, length, label, propagationTimeSignal, timeTag->getTotalTime());
    }
    else
        throw cRuntimeError("Unknown tag");
}

} // namespace queueing
} // namespace inet

