//
// Copyright (C) 2015 OpenSim Ltd.
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

#ifndef __INET_IPV6SOCKET_H
#define __INET_IPV6SOCKET_H

#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/Protocol.h"
#include "inet/networklayer/contract/INetworkSocket.h"

namespace inet {

/**
 * Thie class implements a raw IPv6 socket.
 */
class INET_API Ipv6Socket : public INetworkSocket
{
  public:
    class INET_API ICallback : public INetworkSocket::ICallback
    {
      public:
        virtual void socketDataArrived(INetworkSocket *socket, Packet *packet) override { socketDataArrived(check_and_cast<Ipv6Socket *>(socket), packet); }
        virtual void socketDataArrived(Ipv6Socket *socket, Packet *packet) = 0;
    };
  protected:
    bool bound = false;
    int socketId = -1;
    INetworkSocket::ICallback *cb = nullptr;
    void *userData = nullptr;
    cGate *outputGate = nullptr;

  protected:
    void sendToOutput(cMessage *message);

  public:
    Ipv6Socket(cGate *outputGate = nullptr);
    virtual ~Ipv6Socket() {}

    void *getUserData() const { return userData; }
    void setUserData(void *userData) { this->userData = userData; }

    virtual const Protocol *getNetworkProtocol() const override { return &Protocol::ipv6; }

    /**
     * Returns the internal socket Id.
     */
    int getSocketId() const override { return socketId; }

    virtual bool belongsToSocket(cMessage *msg) const override;

    /**
     * Sets a callback object, to be used with processMessage().
     * This callback object may be your simple module itself (if it
     * multiply inherits from ICallback too, that is you
     * declared it as
     * <pre>
     * class MyAppModule : public cSimpleModule, public Ipv6Socket::ICallback
     * </pre>
     * and redefined the necessary virtual functions; or you may use
     * dedicated class (and objects) for this purpose.
     *
     * Ipv6Socket doesn't delete the callback object in the destructor
     * or on any other occasion.
     */
    virtual void setCallback(INetworkSocket::ICallback *cb) override;

    virtual void processMessage(cMessage *msg) override;

    /**
     * Sets the gate on which to send raw packets. Must be invoked before socket
     * can be used. Example: <tt>socket.setOutputGate(gate("ipOut"));</tt>
     */
    void setOutputGate(cGate *outputGate) { this->outputGate = outputGate; }

    /**
     * Bind the socket to a protocol.
     */
    void bind(const Protocol *protocol) override;

    /**
     * Sends a data packet.
     */
    void send(Packet *packet) override;

    /**
     * Unbinds the socket. Once closed, a closed socket may be bound to another
     * (or the same) protocol, and reused.
     */
    void close() override;
};

} // namespace inet

#endif // ifndef __INET_IPV6SOCKET_H

