#ifndef Net_DatagramSocketImpl_INCLUDED
#define Net_DatagramSocketImpl_INCLUDED


#include "Net.h"
#include "SocketImpl.h"


namespace pi {


class PIL_API DatagramSocketImpl: public SocketImpl
    /// This class implements an UDP socket.
{
public:
    DatagramSocketImpl();
        /// Creates a DatagramSocketImpl.
        ///
        /// If the system supports IPv6, the socket will
        /// be an IPv6 socket. Otherwise, it will be
        /// an IPv4 socket.

    explicit DatagramSocketImpl(IPAddress::Family family);
        /// Creates an unconnected datagram socket.
        ///
        /// The socket will be created for the
        /// given address family.

    DatagramSocketImpl(pil_socket_t sockfd);
        /// Creates a StreamSocketImpl using the given native socket.

protected:
    void init(int af);

    ~DatagramSocketImpl();
};


} // namespace pi


#endif // Net_DatagramSocketImpl_INCLUDED
