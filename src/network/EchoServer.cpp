#include "EchoServer.h"
#include "StreamSocket.h"
#include "SocketAddress.h"
#include "base/Time/Timespan.h"
#include <iostream>

namespace pi{

EchoServer::EchoServer():
    _socket(SocketAddress()),
    _thread("EchoServer"),
    _stop(false)
{
    _thread.start(*this);
    _ready.wait();
}


EchoServer::~EchoServer()
{
    _stop = true;
    _thread.join();
}


pi::UInt16 EchoServer::port() const
{
    return _socket.address().port();
}


void EchoServer::run()
{
    _ready.set();
    pi::Timespan span(250000);
    while (!_stop)
    {
        if (_socket.poll(span, Socket::SELECT_READ))
        {
            StreamSocket ss = _socket.acceptConnection();
            try
            {
                char buffer[256];
                int n = ss.receiveBytes(buffer, sizeof(buffer));
                while (n > 0 && !_stop)
                {
                    ss.sendBytes(buffer, n);
                    n = ss.receiveBytes(buffer, sizeof(buffer));
                }
            }
            catch (pi::Exception& exc)
            {
                std::cerr << "EchoServer: " << exc.displayText() << std::endl;
            }
        }
    }
}

}
