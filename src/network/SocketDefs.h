#ifndef PIL_SocketDefs_INCLUDED
#define PIL_SocketDefs_INCLUDED


#define PIL_ENOERR 0


#if defined(PIL_OS_FAMILY_WINDOWS)
    #include "PIL/UnWindows.h"
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define PIL_INVALID_SOCKET  INVALID_SOCKET
    #define PIL_socket_t        SOCKET
    #define PIL_socklen_t       int
    #define pil_ioctl_request_t int
    #define pil_closesocket(s)  closesocket(s)
    #define PIL_EINTR           WSAEINTR
    #define PIL_EACCES          WSAEACCES
    #define PIL_EFAULT          WSAEFAULT
    #define PIL_EINVAL          WSAEINVAL
    #define PIL_EMFILE          WSAEMFILE
    #define PIL_EAGAIN          WSAEWOULDBLOCK
    #define PIL_EWOULDBLOCK     WSAEWOULDBLOCK
    #define PIL_EINPROGRESS     WSAEINPROGRESS
    #define PIL_EALREADY        WSAEALREADY
    #define PIL_ENOTSOCK        WSAENOTSOCK
    #define PIL_EDESTADDRREQ    WSAEDESTADDRREQ
    #define PIL_EMSGSIZE        WSAEMSGSIZE
    #define PIL_EPROTOTYPE      WSAEPROTOTYPE
    #define PIL_ENOPROTOOPT     WSAENOPROTOOPT
    #define PIL_EPROTONOSUPPORT WSAEPROTONOSUPPORT
    #define PIL_ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
    #define PIL_ENOTSUP         WSAEOPNOTSUPP
    #define PIL_EPFNOSUPPORT    WSAEPFNOSUPPORT
    #define PIL_EAFNOSUPPORT    WSAEAFNOSUPPORT
    #define PIL_EADDRINUSE      WSAEADDRINUSE
    #define PIL_EADDRNOTAVAIL   WSAEADDRNOTAVAIL
    #define PIL_ENETDOWN        WSAENETDOWN
    #define PIL_ENETUNREACH     WSAENETUNREACH
    #define PIL_ENETRESET       WSAENETRESET
    #define PIL_ECONNABORTED    WSAECONNABORTED
    #define PIL_ECONNRESET      WSAECONNRESET
    #define PIL_ENOBUFS         WSAENOBUFS
    #define PIL_EISCONN         WSAEISCONN
    #define PIL_ENOTCONN        WSAENOTCONN
    #define PIL_ESHUTDOWN       WSAESHUTDOWN
    #define PIL_ETIMEDOUT       WSAETIMEDOUT
    #define PIL_ECONNREFUSED    WSAECONNREFUSED
    #define PIL_EHOSTDOWN       WSAEHOSTDOWN
    #define PIL_EHOSTUNREACH    WSAEHOSTUNREACH
    #define PIL_ESYSNOTREADY    WSASYSNOTREADY
    #define PIL_ENOTINIT        WSANOTINITIALISED
    #define PIL_HOST_NOT_FOUND  WSAHOST_NOT_FOUND
    #define PIL_TRY_AGAIN       WSATRY_AGAIN
    #define PIL_NO_RECOVERY     WSANO_RECOVERY
    #define PIL_NO_DATA         WSANO_DATA
    #ifndef ADDRESS_FAMILY
        #define ADDRESS_FAMILY USHORT
    #endif
#elif defined(PIL_VXWORKS)
    #include <hostLib.h>
    #include <ifLib.h>
    #include <inetLib.h>
    #include <ioLib.h>
    #include <resolvLib.h>
    #include <types.h>
    #include <socket.h>
    #include <netinet/tcp.h>
    #define PIL_INVALID_SOCKET  -1
    #define pil_socket_t        int
    #define pil_socklen_t       int
    #define pil_ioctl_request_t int
    #define pil_closesocket(s)  ::close(s)
    #define PIL_EINTR           EINTR
    #define PIL_EACCES          EACCES
    #define PIL_EFAULT          EFAULT
    #define PIL_EINVAL          EINVAL
    #define PIL_EMFILE          EMFILE
    #define PIL_EAGAIN          EAGAIN
    #define PIL_EWOULDBLOCK     EWOULDBLOCK
    #define PIL_EINPROGRESS     EINPROGRESS
    #define PIL_EALREADY        EALREADY
    #define PIL_ENOTSOCK        ENOTSOCK
    #define PIL_EDESTADDRREQ    EDESTADDRREQ
    #define PIL_EMSGSIZE        EMSGSIZE
    #define PIL_EPROTOTYPE      EPROTOTYPE
    #define PIL_ENOPROTOOPT     ENOPROTOOPT
    #define PIL_EPROTONOSUPPORT EPROTONOSUPPORT
    #define PIL_ESOCKTNOSUPPORT ESOCKTNOSUPPORT
    #define PIL_ENOTSUP         ENOTSUP
    #define PIL_EPFNOSUPPORT    EPFNOSUPPORT
    #define PIL_EAFNOSUPPORT    EAFNOSUPPORT
    #define PIL_EADDRINUSE      EADDRINUSE
    #define PIL_EADDRNOTAVAIL   EADDRNOTAVAIL
    #define PIL_ENETDOWN        ENETDOWN
    #define PIL_ENETUNREACH     ENETUNREACH
    #define PIL_ENETRESET       ENETRESET
    #define PIL_ECONNABORTED    ECONNABORTED
    #define PIL_ECONNRESET      ECONNRESET
    #define PIL_ENOBUFS         ENOBUFS
    #define PIL_EISCONN         EISCONN
    #define PIL_ENOTCONN        ENOTCONN
    #define PIL_ESHUTDOWN       ESHUTDOWN
    #define PIL_ETIMEDOUT       ETIMEDOUT
    #define PIL_ECONNREFUSED    ECONNREFUSED
    #define PIL_EHOSTDOWN       EHOSTDOWN
    #define PIL_EHOSTUNREACH    EHOSTUNREACH
    #define PIL_ESYSNOTREADY    -4
    #define PIL_ENOTINIT        -5
    #define PIL_HOST_NOT_FOUND  HOST_NOT_FOUND
    #define PIL_TRY_AGAIN       TRY_AGAIN
    #define PIL_NO_RECOVERY     NO_RECOVERY
    #define PIL_NO_DATA         NO_DATA
#elif defined(PIL_OS_FAMILY_UNIX) || defined(PIL_OS_FAMILY_VMS)
    #include <unistd.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #if PIL_OS != PIL_OS_HPUX
        #include <sys/select.h>
    #endif
    #include <sys/ioctl.h>
    #if defined(PIL_OS_FAMILY_VMS)
        #include <inet.h>
    #else
        #include <arpa/inet.h>
    #endif
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    #if defined(PIL_OS_FAMILY_UNIX)
        #if (PIL_OS == PIL_OS_LINUX)
            // Net/src/NetworkInterface.cpp changed #include <linux/if.h> to #include <net/if.h>
            // no more conflict, can use #include <net/if.h> here
            #include <net/if.h>
        #elif (PIL_OS == PIL_OS_HPUX)
            extern "C"
            {
                #include <net/if.h>
            }
        #else
            #include <net/if.h>
        #endif
    #endif
    #if (PIL_OS == PIL_OS_SOLARIS) || (PIL_OS == PIL_OS_MAC_OS_X)
        #include <sys/sockio.h>
        #include <sys/filio.h>
    #endif
    #define PIL_INVALID_SOCKET  -1
    #define pil_socket_t        int
    #define pil_socklen_t       socklen_t
    #define pil_fcntl_request_t int
    #if defined(PIL_OS_FAMILY_BSD)
        #define pil_ioctl_request_t unsigned long
    #else
        #define pil_ioctl_request_t int
    #endif
    #define pil_closesocket(s)  ::close(s)
    #define PIL_EINTR           EINTR
    #define PIL_EACCES          EACCES
    #define PIL_EFAULT          EFAULT
    #define PIL_EINVAL          EINVAL
    #define PIL_EMFILE          EMFILE
    #define PIL_EAGAIN          EAGAIN
    #define PIL_EWOULDBLOCK     EWOULDBLOCK
    #define PIL_EINPROGRESS     EINPROGRESS
    #define PIL_EALREADY        EALREADY
    #define PIL_ENOTSOCK        ENOTSOCK
    #define PIL_EDESTADDRREQ    EDESTADDRREQ
    #define PIL_EMSGSIZE        EMSGSIZE
    #define PIL_EPROTOTYPE      EPROTOTYPE
    #define PIL_ENOPROTOOPT     ENOPROTOOPT
    #define PIL_EPROTONOSUPPORT EPROTONOSUPPORT
    #if defined(ESOCKTNOSUPPORT)
        #define PIL_ESOCKTNOSUPPORT ESOCKTNOSUPPORT
    #else
        #define PIL_ESOCKTNOSUPPORT -1
    #endif
    #define PIL_ENOTSUP         ENOTSUP
    #define PIL_EPFNOSUPPORT    EPFNOSUPPORT
    #define PIL_EAFNOSUPPORT    EAFNOSUPPORT
    #define PIL_EADDRINUSE      EADDRINUSE
    #define PIL_EADDRNOTAVAIL   EADDRNOTAVAIL
    #define PIL_ENETDOWN        ENETDOWN
    #define PIL_ENETUNREACH     ENETUNREACH
    #define PIL_ENETRESET       ENETRESET
    #define PIL_ECONNABORTED    ECONNABORTED
    #define PIL_ECONNRESET      ECONNRESET
    #define PIL_ENOBUFS         ENOBUFS
    #define PIL_EISCONN         EISCONN
    #define PIL_ENOTCONN        ENOTCONN
    #if defined(ESHUTDOWN)
        #define PIL_ESHUTDOWN   ESHUTDOWN
    #else
        #define PIL_ESHUTDOWN   -2
    #endif
    #define PIL_ETIMEDOUT       ETIMEDOUT
    #define PIL_ECONNREFUSED    ECONNREFUSED
    #if defined(EHOSTDOWN)
        #define PIL_EHOSTDOWN   EHOSTDOWN
    #else
        #define PIL_EHOSTDOWN   -3
    #endif
    #define PIL_EHOSTUNREACH    EHOSTUNREACH
    #define PIL_ESYSNOTREADY    -4
    #define PIL_ENOTINIT        -5
    #define PIL_HOST_NOT_FOUND  HOST_NOT_FOUND
    #define PIL_TRY_AGAIN       TRY_AGAIN
    #define PIL_NO_RECOVERY     NO_RECOVERY
    #define PIL_NO_DATA         NO_DATA
#endif


#if defined(PIL_OS_FAMILY_BSD) || (PIL_OS == PIL_OS_TRU64) || (PIL_OS == PIL_OS_AIX) || (PIL_OS == PIL_OS_IRIX) || (PIL_OS == PIL_OS_QNX) || (PIL_OS == PIL_OS_VXWORKS)
    #define PIL_HAVE_SALEN 1
#endif


#if PIL_OS != PIL_OS_VXWORKS && !defined(PIL_NET_NO_ADDRINFO)
    #define PIL_HAVE_ADDRINFO 1
#endif


#if (PIL_OS == PIL_OS_HPUX) || (PIL_OS == PIL_OS_SOLARIS) || (PIL_OS == PIL_OS_WINDOWS_CE) || (PIL_OS == PIL_OS_CYGWIN)
    #define PIL_BROKEN_TIMEOUTS 1
#endif


#if defined(PIL_HAVE_ADDRINFO)
    #ifndef AI_PASSIVE
        #define AI_PASSIVE 0
    #endif
    #ifndef AI_CANONNAME
        #define AI_CANONNAME 0
    #endif
    #ifndef AI_NUMERICHOST
        #define AI_NUMERICHOST 0
    #endif
    #ifndef AI_NUMERICSERV
        #define AI_NUMERICSERV 0
    #endif
    #ifndef AI_ALL
        #define AI_ALL 0
    #endif
    #ifndef AI_ADDRCONFIG
        #define AI_ADDRCONFIG 0
    #endif
    #ifndef AI_V4MAPPED
        #define AI_V4MAPPED 0
    #endif
#endif


#if defined(PIL_HAVE_SALEN)
    #define pil_set_sa_len(pSA, len)  (pSA)->sa_len   = (len)
    #define pil_set_sin_len(pSA)      (pSA)->sin_len  = sizeof(struct sockaddr_in)
    #if defined(PIL_HAVE_IPv6)
        #define PIL_set_sin6_len(pSA) (pSA)->sin6_len = sizeof(struct sockaddr_in6)
    #endif
#else
    #define pil_set_sa_len(pSA, len) (void) 0
    #define pil_set_sin_len(pSA)     (void) 0
    #define pil_set_sin6_len(pSA)    (void) 0
#endif


#ifndef INADDR_NONE
    #define INADDR_NONE 0xffffffff
#endif

#ifndef INADDR_ANY
    #define INADDR_ANY 0x00000000
#endif

#ifndef INADDR_BROADCAST
    #define INADDR_BROADCAST 0xffffffff
#endif

#ifndef INADDR_LOOPBACK
    #define INADDR_LOOPBACK 0x7f000001
#endif

#ifndef INADDR_UNSPEC_GROUP
    #define INADDR_UNSPEC_GROUP 0xe0000000
#endif

#ifndef INADDR_ALLHOSTS_GROUP
    #define INADDR_ALLHOSTS_GROUP 0xe0000001
#endif

#ifndef INADDR_ALLRTRS_GROUP
    #define INADDR_ALLRTRS_GROUP 0xe0000002
#endif

#ifndef INADDR_MAX_LOCAL_GROUP
    #define INADDR_MAX_LOCAL_GROUP 0xe00000ff
#endif

#if defined(PIL_ARCH_BIG_ENDIAN)
    #define pil_ntoh_16(x) (x)
    #define pil_ntoh_32(x) (x)
#else
    #define pil_ntoh_16(x) \
        ((((x) >> 8) & 0x00ff) | (((x) << 8) & 0xff00))
    #define pil_ntoh_32(x) \
        ((((x) >> 24) & 0x000000ff) | (((x) >> 8) & 0x0000ff00) | (((x) << 8) & 0x00ff0000) | (((x) << 24) & 0xff000000))
#endif
#define pil_hton_16(x) pil_ntoh_16(x)
#define pil_hton_32(x) pil_ntoh_32(x)


#if !defined(s6_addr16)
    #if defined(PIL_OS_FAMILY_WINDOWS)
        #define s6_addr16 u.Word
    #else
        #define s6_addr16 __u6_addr.__u6_addr16
    #endif
#endif


#if !defined(s6_addr32)
    #if defined(PIL_OS_FAMILY_UNIX)
        #if (PIL_OS == PIL_OS_SOLARIS)
            #define s6_addr32 _S6_un._S6_u32
        #else
            #define s6_addr32 __u6_addr.__u6_addr32
        #endif
    #endif
#endif


#endif // Net_SocketDefs_INCLUDED
