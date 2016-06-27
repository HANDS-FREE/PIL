#ifndef EchoServer_INCLUDED
#define EchoServer_INCLUDED


#include "Net.h"
#include "ServerSocket.h"
#include "base/Thread/Thread.h"
#include "base/Thread/Event.h"

namespace pi{

class EchoServer: public pi::Runnable
    /// A simple sequential echo server.
{
public:
    EchoServer();
        /// Creates the EchoServer.

    ~EchoServer();
        /// Destroys the EchoServer.

    pi::UInt16 port() const;
        /// Returns the port the echo server is
        /// listening on.

    void run();
        /// Does the work.

private:
    ServerSocket _socket;
    Thread       _thread;
    Event        _ready;
    bool         _stop;
};

}


#endif // EchoServer_INCLUDED
