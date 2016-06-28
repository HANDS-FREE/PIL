#include "UDPEchoServer.h"
#include "SocketAddress.h"
#include "base/Time/Timespan.h"
#include <iostream>

namespace pi {


UDPEchoServer::UDPEchoServer():
    _thread("UDPEchoServer"),
    _stop(false)
{
    _socket.bind(SocketAddress(), true);
    _thread.start(this);
    _ready.wait();
}


UDPEchoServer::UDPEchoServer(const SocketAddress& sa):
    _thread("UDPEchoServer"),
    _stop(false)
{
    _socket.bind(sa, true);
    _thread.start(this);
    _ready.wait();
}


UDPEchoServer::~UDPEchoServer()
{
    _stop = true;
    _thread.join();
}


UInt16 UDPEchoServer::port() const
{
    return _socket.address().port();
}


void UDPEchoServer::run()
{
    Timespan span(250000);
    while (!_stop)
    {
        _ready.set();
        if (_socket.poll(span, Socket::SELECT_READ))
        {
            try
            {
                char buffer[256];
                SocketAddress sender;
                int n = _socket.receiveFrom(buffer, sizeof(buffer), sender);
                n = _socket.sendTo(buffer, n, sender);
            }
            catch (Exception& exc)
            {
                std::cerr << "UDPEchoServer: " << exc.displayText() << std::endl;
            }
        }
    }
}


SocketAddress UDPEchoServer::address() const
{
    return _socket.address();
}

}
