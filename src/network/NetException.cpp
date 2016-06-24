//
// NetException.cpp
//
// $Id: //PIL/1.4/Net/src/NetException.cpp#4 $
//
// Library: Net
// Package: NetCore
// Module:  NetException
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "NetException.h"
#include <typeinfo>

namespace pi {

PIL_IMPLEMENT_EXCEPTION(NetException, IOException, "Net Exception")
PIL_IMPLEMENT_EXCEPTION(InvalidAddressException, NetException, "Invalid address")
PIL_IMPLEMENT_EXCEPTION(InvalidSocketException, NetException, "Invalid socket")
PIL_IMPLEMENT_EXCEPTION(ServiceNotFoundException, NetException, "Service not found")
PIL_IMPLEMENT_EXCEPTION(ConnectionAbortedException, NetException, "Software caused connection abort")
PIL_IMPLEMENT_EXCEPTION(ConnectionResetException, NetException, "Connection reset by peer")
PIL_IMPLEMENT_EXCEPTION(ConnectionRefusedException, NetException, "Connection refused")
PIL_IMPLEMENT_EXCEPTION(DNSException, NetException, "DNS error")
PIL_IMPLEMENT_EXCEPTION(HostNotFoundException, DNSException, "Host not found")
PIL_IMPLEMENT_EXCEPTION(NoAddressFoundException, DNSException, "No address found")
PIL_IMPLEMENT_EXCEPTION(InterfaceNotFoundException, NetException, "Interface not found")
PIL_IMPLEMENT_EXCEPTION(NoMessageException, NetException, "No message received")
PIL_IMPLEMENT_EXCEPTION(MessageException, NetException, "Malformed message")
PIL_IMPLEMENT_EXCEPTION(MultipartException, MessageException, "Malformed multipart message")
PIL_IMPLEMENT_EXCEPTION(HTTPException, NetException, "HTTP Exception")
PIL_IMPLEMENT_EXCEPTION(NotAuthenticatedException, HTTPException, "No authentication information found")
PIL_IMPLEMENT_EXCEPTION(UnsupportedRedirectException, HTTPException, "Unsupported HTTP redirect (protocol change)")
PIL_IMPLEMENT_EXCEPTION(FTPException, NetException, "FTP Exception")
PIL_IMPLEMENT_EXCEPTION(SMTPException, NetException, "SMTP Exception")
PIL_IMPLEMENT_EXCEPTION(POP3Exception, NetException, "POP3 Exception")
PIL_IMPLEMENT_EXCEPTION(ICMPException, NetException, "ICMP Exception")
PIL_IMPLEMENT_EXCEPTION(NTPException, NetException, "NTP Exception")
PIL_IMPLEMENT_EXCEPTION(HTMLFormException, NetException, "HTML Form Exception")
PIL_IMPLEMENT_EXCEPTION(WebSocketException, NetException, "WebSocket Exception")
PIL_IMPLEMENT_EXCEPTION(UnsupportedFamilyException, NetException, "Unknown or unsupported socket family.")


} // namespace pi
