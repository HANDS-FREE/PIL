#ifndef MulticastEchoServer_INCLUDED
#define MulticastEchoServer_INCLUDED


#include "Net.h"


#ifdef PIL_NET_HAS_INTERFACE


#include "MulticastSocket.h"
#include "SocketAddress.h"
#include "NetworkInterface.h"
#include "base/Thread/Thread.h"
#include "base/Thread/Event.h"

namespace pi{

class MulticastEchoServer: public pi::Runnable
    /// A simple sequential Multicast echo server.
{
public:
    MulticastEchoServer();
        /// Creates the MulticastEchoServer.

    ~MulticastEchoServer();
        /// Destroys the MulticastEchoServer.

    pi::UInt16 port() const;
        /// Returns the port the echo server is
        /// listening on.

    void run();
        /// Does the work.

    const pi::SocketAddress& group() const;
        /// Returns the group address where the server listens.

    const pi::NetworkInterface& interfc() const;
        /// Returns the network interface for multicasting.

protected:
    static pi::NetworkInterface findInterface();
        /// Finds an appropriate network interface for
        /// multicasting.

private:
    pi::MulticastSocket  _socket;
    pi::SocketAddress    _group;
    pi::NetworkInterface _if;
    pi::Thread _thread;
    pi::Event  _ready;
    bool         _stop;
};

}

#endif // POCO_NET_HAS_INTERFACE


#endif // MulticastEchoServer_INCLUDED
