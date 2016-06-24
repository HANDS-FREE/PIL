#ifndef PIL_Net_INCLUDED
#define PIL_Net_INCLUDED

#include "base/Environment.h"


// Automatically link Net library.
//
#if defined(_MSC_VER)
    #if !defined(PIL_NO_AUTOMATIC_LIBS) && !defined(Net_EXPORTS)
        #pragma comment(lib, "pi_network" PIL_LIB_SUFFIX)
    #endif
#endif


// Default to enabled IPv6 support if not explicitly disabled
#if !defined(PIL_NET_NO_IPv6) && !defined (PIL_HAVE_IPv6)
    #define PIL_HAVE_IPv6
#elif defined(PIL_NET_NO_IPv6) && defined (PIL_HAVE_IPv6)
    #undef PIL_HAVE_IPv6
#endif // PIL_NET_NO_IPv6, PIL_HAVE_IPv6


namespace PIL {

void PIL_API initializeNetwork();
    /// Initialize the network subsystem.
    /// (Windows only, no-op elsewhere)


void PIL_API uninitializeNetwork();
    /// Uninitialize the network subsystem.
    /// (Windows only, no-op elsewhere)


} // namespace pi


//
// Automate network initialization (only relevant on Windows).
//

#if defined(PIL_OS_FAMILY_WINDOWS) && !defined(PIL_NO_AUTOMATIC_LIB_INIT) && !defined(__GNUC__)

extern "C" const struct PIL_API NetworkInitializer PILNetworkInitializer;

#if defined(Net_EXPORTS)
    #if defined(_WIN64) || defined(_WIN32_WCE)
        #define PIL_NET_FORCE_SYMBOL(s) __pragma(comment (linker, "/export:"#s))
    #elif defined(_WIN32)
        #define PIL_NET_FORCE_SYMBOL(s) __pragma(comment (linker, "/export:_"#s))
    #endif
#else  // !Net_EXPORTS
    #if defined(_WIN64) || defined(_WIN32_WCE)
        #define PIL_NET_FORCE_SYMBOL(s) __pragma(comment (linker, "/include:"#s))
    #elif defined(_WIN32)
        #define PIL_NET_FORCE_SYMBOL(s) __pragma(comment (linker, "/include:_"#s))
    #endif
#endif // Net_EXPORTS

PIL_NET_FORCE_SYMBOL(PILNetworkInitializer)

#endif // PIL_OS_FAMILY_WINDOWS


//
// Define PIL_NET_HAS_INTERFACE for platforms that have network interface detection implemented.
//
#if defined(PIL_OS_FAMILY_WINDOWS) || (PIL_OS == PIL_OS_LINUX) || defined(PIL_OS_FAMILY_BSD) || (PIL_OS == PIL_OS_SOLARIS) || (PIL_OS == PIL_OS_QNX)
    #define PIL_NET_HAS_INTERFACE
#endif


#endif // Net_Net_INCLUDED
