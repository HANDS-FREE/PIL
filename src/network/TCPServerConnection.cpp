#include "TCPServerConnection.h"
#include "base/Debug/Exception.h"

namespace pi {


TCPServerConnection::TCPServerConnection(const StreamSocket& socket):
    _socket(socket)
{
}


TCPServerConnection::~TCPServerConnection()
{
}


void TCPServerConnection::start()
{
    try
    {
        run();
    }
    catch (Exception& exc)
    {
    }
    catch (std::exception& exc)
    {
    }
    catch (...)
    {
    }
}


}  // namespace pi
