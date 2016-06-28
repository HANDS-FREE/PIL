#ifndef UDPEchoServer_INCLUDED
#define UDPEchoServer_INCLUDED


#include "Net.h"
#include "DatagramSocket.h"
#include "SocketAddress.h"
#include "base/Thread/Thread.h"
#include "base/Thread/Event.h"

namespace pi{

class UDPEchoServer: public Runnable
    /// A simple sequential UDP echo server.
{
public:
    UDPEchoServer();
        /// Creates the UDPEchoServer.

    UDPEchoServer(const SocketAddress& sa);
        /// Creates the UDPEchoServer and binds it to
        /// the given address.

    ~UDPEchoServer();
        /// Destroys the UDPEchoServer.

    UInt16 port() const;
        /// Returns the port the echo server is
        /// listening on.

    SocketAddress address() const;
        /// Returns the address of the server.

    void run();
        /// Does the work.

private:
    DatagramSocket _socket;
    Thread _thread;
    Event  _ready;
    bool         _stop;
};

}

#endif // UDPEchoServer_INCLUDED
