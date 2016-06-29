#include "SocketAddress.h"
#include "IPAddress.h"
#include "NetException.h"
#include "DNS.h"
#include "base/Types/RefCountedObject.h"
#include "base/Utils/utils_str.h"
#include <algorithm>
#include <cstring>

namespace pi {


struct AFLT
{
    bool operator () (const IPAddress& a1, const IPAddress& a2)
    {
        return a1.af() < a2.af();
    }
};


//
// SocketAddress
//


SocketAddress::SocketAddress()
{
    newIPv4();
}


SocketAddress::SocketAddress(const IPAddress& hostAddress, pi::UInt16 portNumber)
{
    init(hostAddress, portNumber);
}


SocketAddress::SocketAddress(pi::UInt16 portNumber)
{
    init(IPAddress(), portNumber);
}


SocketAddress::SocketAddress(const std::string& hostAddress, pi::UInt16 portNumber)
{
    init(hostAddress, portNumber);
}


SocketAddress::SocketAddress(const std::string& hostAddress, const std::string& portNumber)
{
    init(hostAddress, resolveService(portNumber));
}


SocketAddress::SocketAddress(const std::string& hostAndPort)
{
    pi_assert (!hostAndPort.empty());

    std::string host;
    std::string port;
    std::string::const_iterator it  = hostAndPort.begin();
    std::string::const_iterator end = hostAndPort.end();
    if (*it == '[')
    {
        ++it;
        while (it != end && *it != ']') host += *it++;
        if (it == end) throw InvalidArgumentException("Malformed IPv6 address");
        ++it;
    }
    else
    {
        while (it != end && *it != ':') host += *it++;
    }
    if (it != end && *it == ':')
    {
        ++it;
        while (it != end) port += *it++;
    }
    else throw InvalidArgumentException("Missing port number");
    init(host, resolveService(port));
}


SocketAddress::SocketAddress(const SocketAddress& socketAddress)
{
    if (socketAddress.family() == IPAddress::IPv4)
        newIPv4(reinterpret_cast<const sockaddr_in*>(socketAddress.addr()));
    else
        newIPv6(reinterpret_cast<const sockaddr_in6*>(socketAddress.addr()));
}


SocketAddress::SocketAddress(const struct sockaddr* sockAddr, pil_socklen_t length)
{
    if (length == sizeof(struct sockaddr_in))
        newIPv4(reinterpret_cast<const struct sockaddr_in*>(sockAddr));
#if defined(PIL_HAVE_IPv6)
    else if (length == sizeof(struct sockaddr_in6))
        newIPv6(reinterpret_cast<const struct sockaddr_in6*>(sockAddr));
#endif
    else throw pi::InvalidArgumentException("Invalid address length passed to SocketAddress()");
}


SocketAddress::~SocketAddress()
{
    destruct();
}


bool SocketAddress::operator < (const SocketAddress& socketAddress) const
{
    if (family() < socketAddress.family()) return true;
    if (family() > socketAddress.family()) return false;
    if (host() < socketAddress.host()) return true;
    if (host() > socketAddress.host()) return false;
    return (port() < socketAddress.port());
}


SocketAddress& SocketAddress::operator = (const SocketAddress& socketAddress)
{
    if (&socketAddress != this)
    {
        destruct();
        if (socketAddress.family() == IPAddress::IPv4)
            newIPv4(reinterpret_cast<const sockaddr_in*>(socketAddress.addr()));
        else
            newIPv6(reinterpret_cast<const sockaddr_in6*>(socketAddress.addr()));
    }
    return *this;
}


IPAddress SocketAddress::host() const
{
    return pImpl()->host();
}


pi::UInt16 SocketAddress::port() const
{
    return ntohs(pImpl()->port());
}


pil_socklen_t SocketAddress::length() const
{
    return pImpl()->length();
}


const struct sockaddr* SocketAddress::addr() const
{
    return pImpl()->addr();
}


int SocketAddress::af() const
{
    return pImpl()->af();
}


std::string SocketAddress::toString() const
{
    std::string result;
#if defined(PIL_HAVE_IPv6)
    if (host().family() == IPAddress::IPv6)
        result.append("[");
    result.append(host().toString());
    if (host().family() == IPAddress::IPv6)
        result.append("]");
#endif
    result.append(":");
    result.append(pi::itos(port()));
//    NumberFormatter::append(result, port());
    return result;
}


void SocketAddress::init(const IPAddress& hostAddress, pi::UInt16 portNumber)
{
    if (hostAddress.family() == IPAddress::IPv4)
        newIPv4(hostAddress, portNumber);
#if defined(PIL_HAVE_IPv6)
    else if (hostAddress.family() == IPAddress::IPv6)
        newIPv6(hostAddress, portNumber);
#endif
    else throw pi::NotImplementedException("unsupported IP address family");
}


void SocketAddress::init(const std::string& hostAddress, pi::UInt16 portNumber)
{
//    pi_dbg_info("Initializing %s:%d",hostAddress.c_str(),portNumber);
//    std::cerr<<"initializing "<<hostAddress<<":"<<portNumber<<std::endl;
    IPAddress ip;
    if (IPAddress::tryParse(hostAddress, ip))
    {
        init(ip, portNumber);
//        std::cerr<<"Parsed successfully "<<ip.toString()<<":"<<portNumber<<std::endl;
    }
    else
    {
//        std::cerr<<"Parsing with DNS...\n";
        HostEntry he = DNS::hostByName(hostAddress);
        HostEntry::AddressList addresses = he.addresses();
        if (addresses.size() > 0)
        {
#if defined(PIL_HAVE_IPv6)
            // if we get both IPv4 and IPv6 addresses, prefer IPv4
            std::sort(addresses.begin(), addresses.end(), AFLT());
#endif
            init(addresses[0], portNumber);
//            std::cerr<<"DNS parsed successfully "<<addresses[0].toString()<<":"<<portNumber<<std::endl;
        }
        else throw HostNotFoundException("No address found for host", hostAddress);
    }
}


pi::UInt16 SocketAddress::resolveService(const std::string& service)
{
    unsigned port;
    std::stringstream sst(service);
    sst>>port;
    if (port>0&&port <= 0xFFFF)
    {
        return (UInt16) port;
    }
    else
    {
#if defined(PIL_VXWORKS)
        throw ServiceNotFoundException(service);
#else
        struct servent* se = getservbyname(service.c_str(), NULL);
        if (se)
            return ntohs(se->s_port);
        else
            throw ServiceNotFoundException(service);
#endif
    }
}


}
