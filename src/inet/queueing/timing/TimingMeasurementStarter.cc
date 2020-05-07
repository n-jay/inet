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
#include "inet/common/TimeTag_m.h"
#include "inet/queueing/timing/TimingMeasurementStarter.h"

namespace inet {
namespace queueing {

Define_Module(TimingMeasurementStarter);

static bool matchesString(cMatchExpression& matchExpression, const char *string)
{
    cMatchableString matchableString(string);
    return matchExpression.matches(&matchableString);
}

void TimingMeasurementStarter::initialize(int stage)
{
    PacketFlowBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        offset = b(par("offset"));
        length = b(par("length"));
        label = par("label");
        cMatchExpression measureMatcher;
        measureMatcher.setPattern(par("measure"), false, true, true);
        measureElapsedTime = matchesString(measureMatcher, "elapsedTime");
        measureDelayingTime = matchesString(measureMatcher, "delayingTime");
        measureQueueingTime = matchesString(measureMatcher, "queueingTime");
        measureProcessingTime = matchesString(measureMatcher, "processingTime");
        measureTransmissionTime = matchesString(measureMatcher, "transmissionTime");
        measurePropagationTime = matchesString(measureMatcher, "propagationTime");
    }
}

void TimingMeasurementStarter::processPacket(Packet *packet)
{
    auto& data = packet->removeAll();
    if (*label == '\0') {
        if (measureElapsedTime)
            startMeasurement<ElapsedTimeTag>(packet, data, offset, length, nullptr);
        if (measureDelayingTime)
            startMeasurement<DelayingTimeTag>(packet, data, offset, length, nullptr);
        if (measureQueueingTime)
            startMeasurement<QueueingTimeTag>(packet, data, offset, length, nullptr);
        if (measureProcessingTime)
            startMeasurement<ProcessingTimeTag>(packet, data, offset, length, nullptr);
        if (measureTransmissionTime)
            startMeasurement<TransmissionTimeTag>(packet, data, offset, length, nullptr);
        if (measurePropagationTime)
            startMeasurement<PropagationTimeTag>(packet, data, offset, length, nullptr);
    }
    else {
        if (measureElapsedTime)
            startMeasurement<ElapsedTimeTag>(packet, data, offset, length, label);
        if (measureDelayingTime)
            startMeasurement<DelayingTimeTag>(packet, data, offset, length, label);
        if (measureQueueingTime)
            startMeasurement<QueueingTimeTag>(packet, data, offset, length, label);
        if (measureProcessingTime)
            startMeasurement<ProcessingTimeTag>(packet, data, offset, length, label);
        if (measureTransmissionTime)
            startMeasurement<TransmissionTimeTag>(packet, data, offset, length, label);
        if (measurePropagationTime)
            startMeasurement<PropagationTimeTag>(packet, data, offset, length, label);
    }
    packet->insertAtBack(data);
}

} // namespace queueing
} // namespace inet

