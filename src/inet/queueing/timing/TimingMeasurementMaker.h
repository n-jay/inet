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

#ifndef __INET_TIMINGMEASUREMENTMAKER_H
#define __INET_TIMINGMEASUREMENTMAKER_H

#include "inet/common/TimeTag_m.h"
#include "inet/queueing/base/PacketFlowBase.h"

namespace inet {
namespace queueing {

class INET_API TimingMeasurementMaker : public PacketFlowBase
{
  public:
    static simsignal_t lifeTimeSignal;
    static simsignal_t elapsedTimeSignal;
    static simsignal_t delayingTimeSignal;
    static simsignal_t queueingTimeSignal;
    static simsignal_t processingTimeSignal;
    static simsignal_t transmissionTimeSignal;
    static simsignal_t propagationTimeSignal;

  protected:
    b offset = b(0);
    b length = b(-1);
    bool finish = false;
    const char* label = nullptr;
    cMatchExpression labelMatcher;
    bool measureLifeTime = false;
    bool measureElapsedTime = false;
    bool measureDelayingTime = false;
    bool measureQueueingTime = false;
    bool measureProcessingTime = false;
    bool measureTransmissionTime = false;
    bool measurePropagationTime = false;

  protected:
    virtual void initialize(int stage) override;
    virtual void makeMeasurement(Packet *packet, b offset, b length, const char *label, simsignal_t signal, simtime_t value);
    virtual void makeMeasurement(Packet *packet, b offset, b length, const char *label, const TimeTagBase *timeTag);

  public:
    virtual void processPacket(Packet *packet) override;
};

} // namespace queueing
} // namespace inet

#endif // ifndef __INET_TIMINGMEASUREMENTMAKER_H

