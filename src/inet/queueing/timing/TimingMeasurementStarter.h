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

#ifndef __INET_TIMINGMEASUREMENTSTARTER_H
#define __INET_TIMINGMEASUREMENTSTARTER_H

#include "inet/queueing/base/PacketFlowBase.h"

namespace inet {
namespace queueing {

class INET_API TimingMeasurementStarter : public PacketFlowBase
{
  protected:
    const char *label = nullptr;
    b offset = b(0);
    b length = b(-1);
    bool measureElapsedTime = false;
    bool measureDelayingTime = false;
    bool measureQueueingTime = false;
    bool measureProcessingTime = false;
    bool measureTransmissionTime = false;
    bool measurePropagationTime = false;

  protected:
    virtual void initialize(int stage) override;

    template <typename T>
    void startMeasurement(Packet *packet, const Ptr<Chunk>& data, b offset, b length, const char *label) {
        if (length == b(-1))
            length = data->getChunkLength();
        EV_INFO << "Starting measurement on packet " << packet->getName() << ": "
                << "range (" << offset << ", " << offset + length << "), "
                << "class = " << T::getClassName() << std::endl;
        if (*label == '\0')
            data->addTag<T>(offset, length);
        else {
            // KLUDGE: horrible
            auto flowTagSet = data->getNumTags() == 1 ? data->addTagIfAbsent<FlowTagSet>(offset, length) : const_cast<FlowTagSet *>(check_and_cast<const FlowTagSet *>(data->getTag(1)));
            flowTagSet->insertLabels(label);
            flowTagSet->insertTags(new T());
        }
    }

  public:
    virtual void processPacket(Packet *packet) override;
};

} // namespace queueing
} // namespace inet

#endif // ifndef __INET_TIMINGMEASUREMENTSTARTER_H

