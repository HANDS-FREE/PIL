#include "Net.h"


#include "SocketDefs.h"
#include "NetException.h"


namespace pi{


void PIL_API initializeNetwork()
{
#if defined(PIL_OS_FAMILY_WINDOWS)
    WORD    version = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(version, &data) != 0)
        throw NetException("Failed to initialize network subsystem");
#endif
}


void PIL_API uninitializeNetwork()
{
#if defined(PIL_OS_FAMILY_WINDOWS)
    WSACleanup();
#endif
}


}  // namespace pi


#if defined(PIL_OS_FAMILY_WINDOWS) && !defined(PIL_NO_AUTOMATIC_LIB_INIT)

    struct NetworkInitializer
        /// Network initializer for windows statically
        /// linked library.
    {
        NetworkInitializer()
            /// Calls pi::initializeNetwork();
        {
            pi::initializeNetwork();
        }

        ~NetworkInitializer()
            /// Calls pi::uninitializeNetwork();
        {
            try
            {
                pi::uninitializeNetwork();
            }
            catch (...)
            {
                pil_unexpected();
            }
        }
    };

    const NetworkInitializer PILNetworkInitializer;// global

#endif
