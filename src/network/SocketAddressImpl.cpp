#include "SocketAddressImpl.h"
#include "SocketDefs.h"
#include <cstring>


namespace pi {

//
// SocketAddressImpl
//


SocketAddressImpl::SocketAddressImpl()
{
}


SocketAddressImpl::~SocketAddressImpl()
{
}


//
// IPv4SocketAddressImpl
//


IPv4SocketAddressImpl::IPv4SocketAddressImpl()
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    pil_set_sin_len(&_addr);
}


IPv4SocketAddressImpl::IPv4SocketAddressImpl(const struct sockaddr_in* addr)
{
    std::memcpy(&_addr, addr, sizeof(_addr));
}


IPv4SocketAddressImpl::IPv4SocketAddressImpl(const void* addr, UInt16 port)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    std::memcpy(&_addr.sin_addr, addr, sizeof(_addr.sin_addr));
    _addr.sin_port = port;
}


#if defined(PIL_HAVE_IPv6)


//
// IPv6SocketAddressImpl
//


IPv6SocketAddressImpl::IPv6SocketAddressImpl(const struct sockaddr_in6* addr)
{
    std::memcpy(&_addr, addr, sizeof(_addr));
}


IPv6SocketAddressImpl::IPv6SocketAddressImpl(const void* addr, UInt16 port)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin6_family = AF_INET6;
    pil_set_sin6_len(&_addr);
    std::memcpy(&_addr.sin6_addr, addr, sizeof(_addr.sin6_addr));
    _addr.sin6_port = port;
}


IPv6SocketAddressImpl::IPv6SocketAddressImpl(const void* addr, UInt16 port, UInt32 scope)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin6_family = AF_INET6;
    pil_set_sin6_len(&_addr);
    std::memcpy(&_addr.sin6_addr, addr, sizeof(_addr.sin6_addr));
    _addr.sin6_port = port;
    _addr.sin6_scope_id = scope;
}


#endif // PIL_HAVE_IPv6


}  // namespace pi
