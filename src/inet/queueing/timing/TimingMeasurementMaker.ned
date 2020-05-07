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

package inet.queueing.timing;

import inet.queueing.base.PacketFlowBase;
import inet.queueing.contract.IPacketFlow;

simple TimingMeasurementMaker extends PacketFlowBase like IPacketFlow
{
    parameters:
        int offset @unit(b) = default(0b);
        int length @unit(b) = default(-1b);
        bool finish = default(true);
        string label = default("");
        string measure = default("*");
        @class(TimingMeasurementMaker);
        @display("i=block/timer");
        @signal[lifeTime](type=simtime_t);
        @signal[elapsedTime](type=simtime_t);
        @signal[delayingTime](type=simtime_t);
        @signal[queueingTime](type=simtime_t);
        @signal[processingTime](type=simtime_t);
        @signal[transmissionTime](type=simtime_t);
        @signal[propagationTime](type=simtime_t);
        @statistic[lifeTime](title="Life time"; record=vector,histogram; interpolationmode=none);
        @statistic[elapsedTime](title="Elapsed time"; record=vector,histogram; interpolationmode=none);
        @statistic[delayingTime](title="Delaying time"; record=vector,histogram; interpolationmode=none);
        @statistic[queueingTime](title="Queueing time"; record=vector,histogram; interpolationmode=none);
        @statistic[processingTime](title="Processing time"; record=vector,histogram; interpolationmode=none);
        @statistic[transmissionTime](title="Transmission time"; record=vector,histogram; interpolationmode=none);
        @statistic[propagationTime](title="Propagation time"; record=vector,histogram; interpolationmode=none);
}
