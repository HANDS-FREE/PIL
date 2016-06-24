#ifndef PIL_NetException_INCLUDED
#define PIL_NetException_INCLUDED

#include "base/Debug/Exception.h"


namespace pi {

PIL_DECLARE_EXCEPTION(PIL_API, NetException, pi::IOException)
PIL_DECLARE_EXCEPTION(PIL_API, InvalidAddressException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, InvalidSocketException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, ServiceNotFoundException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, ConnectionAbortedException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, ConnectionResetException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, ConnectionRefusedException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, DNSException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, HostNotFoundException, DNSException)
PIL_DECLARE_EXCEPTION(PIL_API, NoAddressFoundException, DNSException)
PIL_DECLARE_EXCEPTION(PIL_API, InterfaceNotFoundException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, NoMessageException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, MessageException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, MultipartException, MessageException)
PIL_DECLARE_EXCEPTION(PIL_API, HTTPException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, NotAuthenticatedException, HTTPException)
PIL_DECLARE_EXCEPTION(PIL_API, UnsupportedRedirectException, HTTPException)
PIL_DECLARE_EXCEPTION(PIL_API, FTPException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, SMTPException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, POP3Exception, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, ICMPException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, NTPException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, HTMLFormException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, WebSocketException, NetException)
PIL_DECLARE_EXCEPTION(PIL_API, UnsupportedFamilyException, NetException)

} // namespace pi


#endif // PIL_NetException_INCLUDED
