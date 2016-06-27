#ifndef Net_ServerSocketImpl_INCLUDED
#define Net_ServerSocketImpl_INCLUDED

#include "Net.h"
#include "SocketImpl.h"

namespace pi {


class PIL_API ServerSocketImpl: public SocketImpl
    /// This class implements a TCP server socket.
{
public:
    ServerSocketImpl();
        /// Creates the ServerSocketImpl.

protected:
    virtual ~ServerSocketImpl();
        /// Destroys the ServerSocketImpl.
};


}


#endif // Net_ServerSocketImpl_INCLUDED
