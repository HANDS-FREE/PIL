#include "DatagramSocketImpl.h"
#include "NetException.h"

namespace pi {

DatagramSocketImpl::DatagramSocketImpl()
{
    init(AF_INET);
}


DatagramSocketImpl::DatagramSocketImpl(IPAddress::Family family)
{
    if (family == IPAddress::IPv4)
        init(AF_INET);
#if defined(PIL_HAVE_IPv6)
    else if (family == IPAddress::IPv6)
        init(AF_INET6);
#endif
    else throw InvalidArgumentException("Invalid or unsupported address family passed to DatagramSocketImpl");
}


DatagramSocketImpl::DatagramSocketImpl(pil_socket_t sockfd): SocketImpl(sockfd)
{
}


DatagramSocketImpl::~DatagramSocketImpl()
{
}


void DatagramSocketImpl::init(int af)
{
    initSocket(af, SOCK_DGRAM);
}


}  // namespace pi
