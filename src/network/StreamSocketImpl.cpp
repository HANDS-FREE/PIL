#include "StreamSocketImpl.h"
#include "IPAddress.h"
#include "base/Debug/Exception.h"
#include "base/Thread/Thread.h"


namespace pi {


StreamSocketImpl::StreamSocketImpl()
{
}


StreamSocketImpl::StreamSocketImpl(IPAddress::Family family)
{
    if (family == IPAddress::IPv4)
        init(AF_INET);
#if defined(PIL_HAVE_IPv6)
    else if (family == IPAddress::IPv6)
        init(AF_INET6);
#endif
    else throw pi::InvalidArgumentException("Invalid or unsupported address family passed to StreamSocketImpl");
}


StreamSocketImpl::StreamSocketImpl(pil_socket_t sockfd): SocketImpl(sockfd)
{
}


StreamSocketImpl::~StreamSocketImpl()
{
}


int StreamSocketImpl::sendBytes(const void* buffer, int length, int flags)
{
    const char* p = reinterpret_cast<const char*>(buffer);
    int remaining = length;
    int sent = 0;
    bool blocking = getBlocking();
    while (remaining > 0)
    {
        int n = SocketImpl::sendBytes(p, remaining, flags);
        pi_assert_dbg (n >= 0);
        p += n;
        sent += n;
        remaining -= n;
        if (blocking && remaining > 0)
            pi::Thread::yield();
        else
            break;
    }
    return sent;
}


} // namespace pi
