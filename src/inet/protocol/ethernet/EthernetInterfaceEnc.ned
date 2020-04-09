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

package inet.protocol.ethernet;

import inet.linklayer.contract.IEthernetInterface;
import inet.linklayer.ethernet.IEtherEncap;
import inet.linklayer.ieee8021q.IIeee8021qEncap;
import inet.networklayer.common.InterfaceEntry;
import inet.protocol.common.InterPacketGap;
import inet.protocol.common.PacketSerializer;
import inet.protocol.common.PacketTrimmer;
import inet.protocol.common.PaddingInserter;
import inet.protocol.common.SimpleInOut;
import inet.protocol.connectionmanager.ConnectionManager;
import inet.protocol.transceiver.StreamingReceiver;
import inet.protocol.transceiver.StreamingTransmitter;
import inet.queueing.contract.ITrafficConditioner;
import inet.queueing.queue.PacketQueue;
import inet.queueing.server.InstantServer;


module EthernetInterfaceEnc extends InterfaceEntry like IEthernetInterface
{
    parameters:
        @class(InterfaceEntry);
        @lifecycleSupport;
        @display("i=block/ifcard");
        string interfaceTableModule;
        string address = default("auto");
        bool broadcast = default(true);
        bool multicast = default(true);
        bool promiscuous = default(false);  // if true, all packets are received, otherwise only the
                                            // ones with matching destination MAC address
        double bitrate @unit(bps) = default(nan bps);
        int mtu @unit(B) = default(1500B);
        string fcsMode @enum("declared","computed") = default("declared");
        padding.minLength = 60B;
        ifg.duration = 96s/dropUnit(bitrate);
        transmitter.datarate = bitrate;
        receiver.datarate = bitrate;
        addressChecking.promiscuous = promiscuous;
        encap.fcsMode = fcsMode;
        encap.interfaceTableModule = default(absPath(interfaceTableModule));
        ingressTC.interfaceTableModule = default(absPath(interfaceTableModule));
        egressTC.interfaceTableModule = default(absPath(interfaceTableModule));
    gates:
        input upperLayerIn;
        output upperLayerOut;
        inout phys;
    submodules:
        egressTC: <default("OmittedTrafficConditioner")> like ITrafficConditioner {
            @display("p=200,100");
        }
        ingressTC: <default("OmittedTrafficConditioner")> like ITrafficConditioner {
            @display("p=500,100");
        }
        encap: <default("EtherEncap")> like IEtherEncap {
            parameters:
                @display("p=350,150");
        }
        qEncap: <default("OmittedIeee8021qEncap")> like IIeee8021qEncap {
            parameters:
                @display("p=350,250");
        }
        queue: PacketQueue {
            parameters:
                @display("p=200,300");
        }
        server: InstantServer {
            parameters:
                @display("p=200,400");
        }
        fcsDropper: EthernetFcsDropper {
            parameters:
                @display("p=200,500");
        }
        packetTrimmer: PacketTrimmer {
            @display("p=200,600");
        }
        padding: PaddingInserter {
            parameters:
                @display("p=200,700");
        }
        fcsInserter: EthernetFcsInserter {
            parameters:
                @display("p=200,800");
        }
        preambleInserter: EthernetPreambleInserter {
            parameters:
                @display("p=200,900");
        }
        serializer: PacketSerializer {
            parameters:
                @display("p=200,1000");
        }
        ifg: InterPacketGap {
            parameters:
                @display("p=200,1100");
        }
        transmitter: StreamingTransmitter {
            parameters:
                @display("p=200,1200");
        }
        cm: ConnectionManager {
            parameters:
                @display("p=350,1300");
        }
        fcsChecker: EthernetFcsChecker2 {
            parameters:
                @display("p=500,800");
        }
        preambleChecker: EthernetPreambleChecker {
            parameters:
                @display("p=500,1000");
        }
        receiver: StreamingReceiver {
            parameters:
                @display("p=500,1200");
        }
        channelDatarateReader: ChannelDatarateReader {
            @display("p=100,1300");
        }
    connections:
        upperLayerIn --> { @display("m=n"); } --> egressTC.in;
        egressTC.out --> encap.upperLayerIn;

        encap.upperLayerOut --> ingressTC.in;
        ingressTC.out --> { @display("m=n"); } --> upperLayerOut;

        encap.lowerLayerOut --> qEncap.upperLayerIn;
        qEncap.lowerLayerOut --> queue.in;

        queue.out --> server.in;
        server.out --> fcsDropper.in;
        fcsDropper.out --> packetTrimmer.in;
        packetTrimmer.out --> padding.in;
        padding.out --> fcsInserter.in;
        fcsInserter.out --> preambleInserter.in;
        preambleInserter.out --> serializer.in;
        serializer.out --> ifg.in;
        ifg.out --> transmitter.in;
        transmitter.out --> cm.in;
        cm.physOut --> { @display("m=s"); } --> phys$o;

        fcsChecker.out --> qEncap.lowerLayerIn;
        qEncap.upperLayerOut --> encap.lowerLayerIn;

        preambleChecker.out --> fcsChecker.in;
        receiver.out --> preambleChecker.in;
        cm.out --> receiver.in;
        phys$i --> { @display("m=s"); } --> cm.physIn;
}

