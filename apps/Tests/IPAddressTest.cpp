#include <network/IPAddress.h>
#include <network/NetException.h>
#include <base/Utils/TestCase.h>

using namespace std;
using namespace pi;

class IPAddressTest : public TestCase
{
public:
    IPAddressTest():TestCase("IPAdressTest"){}

    virtual void run()
    {
        testStringConv();
        testStringConv6();
        testParse();
        testClassification();
        testMCClassification();
        testClassification6();
        testMCClassification6();
        testRelationals();
        testRelationals6();
        testWildcard();
        testBroadcast();
        testPrefixCons();
        testPrefixLen();
        testOperators();
        testByteOrderMacros();
    }


    void testStringConv();
    void testStringConv6();
    void testParse();
    void testClassification();
    void testMCClassification();
    void testClassification6();
    void testMCClassification6();
    void testRelationals();
    void testRelationals6();
    void testWildcard();
    void testBroadcast();
    void testPrefixCons();
    void testPrefixLen();
    void testOperators();
    void testByteOrderMacros();

};
IPAddressTest ipadressTest;

void IPAddressTest::testStringConv()
{
    IPAddress ia1("127.0.0.1");
    pi_assert (ia1.family() == IPAddress::IPv4);
    pi_assert (ia1.toString() == "127.0.0.1");

    IPAddress ia2("192.168.1.120");
    pi_assert (ia2.family() == IPAddress::IPv4);
    pi_assert (ia2.toString() == "192.168.1.120");

    IPAddress ia3("255.255.255.255");
    pi_assert (ia3.family() == IPAddress::IPv4);
    pi_assert (ia3.toString() == "255.255.255.255");

    IPAddress ia4("0.0.0.0");
    pi_assert (ia4.family() == IPAddress::IPv4);
    pi_assert (ia4.toString() == "0.0.0.0");

    IPAddress ia5(24,  IPAddress::IPv4);
    pi_assert (ia5.family() == IPAddress::IPv4);
    pi_assert (ia5.toString() == "255.255.255.0");
}


void IPAddressTest::testStringConv6()
{
#ifdef PIL_HAVE_IPv6
    IPAddress ia0("::1");
    pi_assert (ia0.family() == IPAddress::IPv6);
    pi_assert (ia0.toString() == "::1");

    IPAddress ia1("1080:0:0:0:8:600:200a:425c");
    pi_assert (ia1.family() == IPAddress::IPv6);
    pi_assert (ia1.toString() == "1080::8:600:200a:425c");

    IPAddress ia2("1080::8:600:200A:425C");
    pi_assert (ia2.family() == IPAddress::IPv6);
    pi_assert (ia2.toString() == "1080::8:600:200a:425c");

    IPAddress ia3("::192.168.1.120");
    pi_assert (ia3.family() == IPAddress::IPv6);
    pi_assert (ia3.toString() == "::192.168.1.120");

    IPAddress ia4("::ffff:192.168.1.120");
    pi_assert (ia4.family() == IPAddress::IPv6);
    pi_assert (ia4.toString() == "::ffff:192.168.1.120");

    IPAddress ia5(64, IPAddress::IPv6);
    pi_assert (ia5.family() == IPAddress::IPv6);
    pi_assert (ia5.toString() == "ffff:ffff:ffff:ffff::");

    IPAddress ia6(32, IPAddress::IPv6);
    pi_assert (ia6.family() == IPAddress::IPv6);
    pi_assert (ia6.toString() == "ffff:ffff::");
#endif
}


void IPAddressTest::testParse()
{
    IPAddress ip;
    pi_assert (IPAddress::tryParse("192.168.1.120", ip));

    pi_assert (!IPAddress::tryParse("192.168.1.280", ip));

    ip = IPAddress::parse("192.168.1.120");
    try
    {
        ip = IPAddress::parse("192.168.1.280");
        pi_assert2(false,"bad address - must throw");
    }
    catch (InvalidAddressException&)
    {
    }
}


void IPAddressTest::testClassification()
{
    IPAddress ip1("0.0.0.0"); // wildcard
    pi_assert (ip1.isWildcard());
    pi_assert (!ip1.isBroadcast());
    pi_assert (!ip1.isLoopback());
    pi_assert (!ip1.isMulticast());
    pi_assert (!ip1.isUnicast());
    pi_assert (!ip1.isLinkLocal());
    pi_assert (!ip1.isSiteLocal());
    pi_assert (!ip1.isWellKnownMC());
    pi_assert (!ip1.isNodeLocalMC());
    pi_assert (!ip1.isLinkLocalMC());
    pi_assert (!ip1.isSiteLocalMC());
    pi_assert (!ip1.isOrgLocalMC());
    pi_assert (!ip1.isGlobalMC());

    IPAddress ip2("255.255.255.255"); // broadcast
    pi_assert (!ip2.isWildcard());
    pi_assert (ip2.isBroadcast());
    pi_assert (!ip2.isLoopback());
    pi_assert (!ip2.isMulticast());
    pi_assert (!ip2.isUnicast());
    pi_assert (!ip2.isLinkLocal());
    pi_assert (!ip2.isSiteLocal());
    pi_assert (!ip2.isWellKnownMC());
    pi_assert (!ip2.isNodeLocalMC());
    pi_assert (!ip2.isLinkLocalMC());
    pi_assert (!ip2.isSiteLocalMC());
    pi_assert (!ip2.isOrgLocalMC());
    pi_assert (!ip2.isGlobalMC());

    IPAddress ip3("127.0.0.1"); // loopback
    pi_assert (!ip3.isWildcard());
    pi_assert (!ip3.isBroadcast());
    pi_assert (ip3.isLoopback());
    pi_assert (!ip3.isMulticast());
    pi_assert (ip3.isUnicast());
    pi_assert (!ip3.isLinkLocal());
    pi_assert (!ip3.isSiteLocal());
    pi_assert (!ip3.isWellKnownMC());
    pi_assert (!ip3.isNodeLocalMC());
    pi_assert (!ip3.isLinkLocalMC());
    pi_assert (!ip3.isSiteLocalMC());
    pi_assert (!ip3.isOrgLocalMC());
    pi_assert (!ip3.isGlobalMC());

    IPAddress ip4("80.122.195.86"); // unicast
    pi_assert (!ip4.isWildcard());
    pi_assert (!ip4.isBroadcast());
    pi_assert (!ip4.isLoopback());
    pi_assert (!ip4.isMulticast());
    pi_assert (ip4.isUnicast());
    pi_assert (!ip4.isLinkLocal());
    pi_assert (!ip4.isSiteLocal());
    pi_assert (!ip4.isWellKnownMC());
    pi_assert (!ip4.isNodeLocalMC());
    pi_assert (!ip4.isLinkLocalMC());
    pi_assert (!ip4.isSiteLocalMC());
    pi_assert (!ip4.isOrgLocalMC());
    pi_assert (!ip4.isGlobalMC());

    IPAddress ip5("169.254.1.20"); // link local unicast
    pi_assert (!ip5.isWildcard());
    pi_assert (!ip5.isBroadcast());
    pi_assert (!ip5.isLoopback());
    pi_assert (!ip5.isMulticast());
    pi_assert (ip5.isUnicast());
    pi_assert (ip5.isLinkLocal());
    pi_assert (!ip5.isSiteLocal());
    pi_assert (!ip5.isWellKnownMC());
    pi_assert (!ip5.isNodeLocalMC());
    pi_assert (!ip5.isLinkLocalMC());
    pi_assert (!ip5.isSiteLocalMC());
    pi_assert (!ip5.isOrgLocalMC());
    pi_assert (!ip5.isGlobalMC());

    IPAddress ip6("192.168.1.120"); // site local unicast
    pi_assert (!ip6.isWildcard());
    pi_assert (!ip6.isBroadcast());
    pi_assert (!ip6.isLoopback());
    pi_assert (!ip6.isMulticast());
    pi_assert (ip6.isUnicast());
    pi_assert (!ip6.isLinkLocal());
    pi_assert (ip6.isSiteLocal());
    pi_assert (!ip6.isWellKnownMC());
    pi_assert (!ip6.isNodeLocalMC());
    pi_assert (!ip6.isLinkLocalMC());
    pi_assert (!ip6.isSiteLocalMC());
    pi_assert (!ip6.isOrgLocalMC());
    pi_assert (!ip6.isGlobalMC());

    IPAddress ip7("10.0.0.138"); // site local unicast
    pi_assert (!ip7.isWildcard());
    pi_assert (!ip7.isBroadcast());
    pi_assert (!ip7.isLoopback());
    pi_assert (!ip7.isMulticast());
    pi_assert (ip7.isUnicast());
    pi_assert (!ip7.isLinkLocal());
    pi_assert (ip7.isSiteLocal());
    pi_assert (!ip7.isWellKnownMC());
    pi_assert (!ip7.isNodeLocalMC());
    pi_assert (!ip7.isLinkLocalMC());
    pi_assert (!ip7.isSiteLocalMC());
    pi_assert (!ip7.isOrgLocalMC());
    pi_assert (!ip7.isGlobalMC());

    IPAddress ip8("172.18.1.200"); // site local unicast
    pi_assert (!ip8.isWildcard());
    pi_assert (!ip8.isBroadcast());
    pi_assert (!ip8.isLoopback());
    pi_assert (!ip8.isMulticast());
    pi_assert (ip8.isUnicast());
    pi_assert (!ip8.isLinkLocal());
    pi_assert (ip8.isSiteLocal());
    pi_assert (!ip8.isWellKnownMC());
    pi_assert (!ip8.isNodeLocalMC());
    pi_assert (!ip8.isLinkLocalMC());
    pi_assert (!ip8.isSiteLocalMC());
    pi_assert (!ip8.isOrgLocalMC());
    pi_assert (!ip8.isGlobalMC());
}


void IPAddressTest::testMCClassification()
{
    IPAddress ip1("224.0.0.100"); // well-known multicast
    pi_assert (!ip1.isWildcard());
    pi_assert (!ip1.isBroadcast());
    pi_assert (!ip1.isLoopback());
    pi_assert (ip1.isMulticast());
    pi_assert (!ip1.isUnicast());
    pi_assert (!ip1.isLinkLocal());
    pi_assert (!ip1.isSiteLocal());
    pi_assert (ip1.isWellKnownMC());
    pi_assert (!ip1.isNodeLocalMC());
    pi_assert (ip1.isLinkLocalMC()); // well known are in the range of link local
    pi_assert (!ip1.isSiteLocalMC());
    pi_assert (!ip1.isOrgLocalMC());
    pi_assert (!ip1.isGlobalMC());

    IPAddress ip2("224.1.0.100"); // link local unicast
    pi_assert (!ip2.isWildcard());
    pi_assert (!ip2.isBroadcast());
    pi_assert (!ip2.isLoopback());
    pi_assert (ip2.isMulticast());
    pi_assert (!ip2.isUnicast());
    pi_assert (!ip2.isLinkLocal());
    pi_assert (!ip2.isSiteLocal());
    pi_assert (!ip2.isWellKnownMC());
    pi_assert (!ip2.isNodeLocalMC());
    pi_assert (ip2.isLinkLocalMC());
    pi_assert (!ip2.isSiteLocalMC());
    pi_assert (!ip2.isOrgLocalMC());
    pi_assert (ip2.isGlobalMC()); // link local fall in the range of global

    IPAddress ip3("239.255.0.100"); // site local unicast
    pi_assert (!ip3.isWildcard());
    pi_assert (!ip3.isBroadcast());
    pi_assert (!ip3.isLoopback());
    pi_assert (ip3.isMulticast());
    pi_assert (!ip3.isUnicast());
    pi_assert (!ip3.isLinkLocal());
    pi_assert (!ip3.isSiteLocal());
    pi_assert (!ip3.isWellKnownMC());
    pi_assert (!ip3.isNodeLocalMC());
    pi_assert (!ip3.isLinkLocalMC());
    pi_assert (ip3.isSiteLocalMC());
    pi_assert (!ip3.isOrgLocalMC());
    pi_assert (!ip3.isGlobalMC());

    IPAddress ip4("239.192.0.100"); // org local unicast
    pi_assert (!ip4.isWildcard());
    pi_assert (!ip4.isBroadcast());
    pi_assert (!ip4.isLoopback());
    pi_assert (ip4.isMulticast());
    pi_assert (!ip4.isUnicast());
    pi_assert (!ip4.isLinkLocal());
    pi_assert (!ip4.isSiteLocal());
    pi_assert (!ip4.isWellKnownMC());
    pi_assert (!ip4.isNodeLocalMC());
    pi_assert (!ip4.isLinkLocalMC());
    pi_assert (!ip4.isSiteLocalMC());
    pi_assert (ip4.isOrgLocalMC());
    pi_assert (!ip4.isGlobalMC());

    IPAddress ip5("224.2.127.254"); // global unicast
    pi_assert (!ip5.isWildcard());
    pi_assert (!ip5.isBroadcast());
    pi_assert (!ip5.isLoopback());
    pi_assert (ip5.isMulticast());
    pi_assert (!ip5.isUnicast());
    pi_assert (!ip5.isLinkLocal());
    pi_assert (!ip5.isSiteLocal());
    pi_assert (!ip5.isWellKnownMC());
    pi_assert (!ip5.isNodeLocalMC());
    pi_assert (ip5.isLinkLocalMC()); // link local fall in the range of global
    pi_assert (!ip5.isSiteLocalMC());
    pi_assert (!ip5.isOrgLocalMC());
    pi_assert (ip5.isGlobalMC());
}


void IPAddressTest::testClassification6()
{
#ifdef PIL_HAVE_IPv6
    IPAddress ip1("::"); // wildcard
    pi_assert (ip1.isWildcard());
    pi_assert (!ip1.isBroadcast());
    pi_assert (!ip1.isLoopback());
    pi_assert (!ip1.isMulticast());
    pi_assert (!ip1.isUnicast());
    pi_assert (!ip1.isLinkLocal());
    pi_assert (!ip1.isSiteLocal());
    pi_assert (!ip1.isWellKnownMC());
    pi_assert (!ip1.isNodeLocalMC());
    pi_assert (!ip1.isLinkLocalMC());
    pi_assert (!ip1.isSiteLocalMC());
    pi_assert (!ip1.isOrgLocalMC());
    pi_assert (!ip1.isGlobalMC());

    IPAddress ip3("::1"); // loopback
    pi_assert (!ip3.isWildcard());
    pi_assert (!ip3.isBroadcast());
    pi_assert (ip3.isLoopback());
    pi_assert (!ip3.isMulticast());
    pi_assert (ip3.isUnicast());
    pi_assert (!ip3.isLinkLocal());
    pi_assert (!ip3.isSiteLocal());
    pi_assert (!ip3.isWellKnownMC());
    pi_assert (!ip3.isNodeLocalMC());
    pi_assert (!ip3.isLinkLocalMC());
    pi_assert (!ip3.isSiteLocalMC());
    pi_assert (!ip3.isOrgLocalMC());
    pi_assert (!ip3.isGlobalMC());

    IPAddress ip4("2001:0db8:85a3:0000:0000:8a2e:0370:7334"); // unicast
    pi_assert (!ip4.isWildcard());
    pi_assert (!ip4.isBroadcast());
    pi_assert (!ip4.isLoopback());
    pi_assert (!ip4.isMulticast());
    pi_assert (ip4.isUnicast());
    pi_assert (!ip4.isLinkLocal());
    pi_assert (!ip4.isSiteLocal());
    pi_assert (!ip4.isWellKnownMC());
    pi_assert (!ip4.isNodeLocalMC());
    pi_assert (!ip4.isLinkLocalMC());
    pi_assert (!ip4.isSiteLocalMC());
    pi_assert (!ip4.isOrgLocalMC());
    pi_assert (!ip4.isGlobalMC());

    IPAddress ip5("fe80::21f:5bff:fec6:6707"); // link local unicast
    pi_assert (!ip5.isWildcard());
    pi_assert (!ip5.isBroadcast());
    pi_assert (!ip5.isLoopback());
    pi_assert (!ip5.isMulticast());
    pi_assert (ip5.isUnicast());
    pi_assert (ip5.isLinkLocal());
    pi_assert (!ip5.isSiteLocal());
    pi_assert (!ip5.isWellKnownMC());
    pi_assert (!ip5.isNodeLocalMC());
    pi_assert (!ip5.isLinkLocalMC());
    pi_assert (!ip5.isSiteLocalMC());
    pi_assert (!ip5.isOrgLocalMC());
    pi_assert (!ip5.isGlobalMC());

    IPAddress ip10("fe80::12"); // link local unicast
    pi_assert (!ip10.isWildcard());
    pi_assert (!ip10.isBroadcast());
    pi_assert (!ip10.isLoopback());
    pi_assert (!ip10.isMulticast());
    pi_assert (ip10.isUnicast());
    pi_assert (ip10.isLinkLocal());
    pi_assert (!ip10.isSiteLocal());
    pi_assert (!ip10.isWellKnownMC());
    pi_assert (!ip10.isNodeLocalMC());
    pi_assert (!ip10.isLinkLocalMC());
    pi_assert (!ip10.isSiteLocalMC());
    pi_assert (!ip10.isOrgLocalMC());
    pi_assert (!ip10.isGlobalMC());

    IPAddress ip6("fec0::21f:5bff:fec6:6707"); // site local unicast (RFC 4291)
    pi_assert (!ip6.isWildcard());
    pi_assert (!ip6.isBroadcast());
    pi_assert (!ip6.isLoopback());
    pi_assert (!ip6.isMulticast());
    pi_assert (ip6.isUnicast());
    pi_assert (!ip6.isLinkLocal());
    pi_assert (ip6.isSiteLocal());
    pi_assert (!ip6.isWellKnownMC());
    pi_assert (!ip6.isNodeLocalMC());
    pi_assert (!ip6.isLinkLocalMC());
    pi_assert (!ip6.isSiteLocalMC());
    pi_assert (!ip6.isOrgLocalMC());
    pi_assert (!ip6.isGlobalMC());

    IPAddress ip7("fc00::21f:5bff:fec6:6707"); // site local unicast (RFC 4193)
    pi_assert (!ip7.isWildcard());
    pi_assert (!ip7.isBroadcast());
    pi_assert (!ip7.isLoopback());
    pi_assert (!ip7.isMulticast());
    pi_assert (ip7.isUnicast());
    pi_assert (!ip7.isLinkLocal());
    pi_assert (ip7.isSiteLocal());
    pi_assert (!ip7.isWellKnownMC());
    pi_assert (!ip7.isNodeLocalMC());
    pi_assert (!ip7.isLinkLocalMC());
    pi_assert (!ip7.isSiteLocalMC());
    pi_assert (!ip7.isOrgLocalMC());
    pi_assert (!ip7.isGlobalMC());
#endif
}


void IPAddressTest::testMCClassification6()
{
#ifdef PIL_HAVE_IPv6
    IPAddress ip1("ff02:0:0:0:0:0:0:c"); // well-known link-local multicast
    pi_assert (!ip1.isWildcard());
    pi_assert (!ip1.isBroadcast());
    pi_assert (!ip1.isLoopback());
    pi_assert (ip1.isMulticast());
    pi_assert (!ip1.isUnicast());
    pi_assert (!ip1.isLinkLocal());
    pi_assert (!ip1.isSiteLocal());
    pi_assert (ip1.isWellKnownMC());
    pi_assert (!ip1.isNodeLocalMC());
    pi_assert (ip1.isLinkLocalMC());
    pi_assert (!ip1.isSiteLocalMC());
    pi_assert (!ip1.isOrgLocalMC());
    pi_assert (!ip1.isGlobalMC());

    IPAddress ip2("ff01:0:0:0:0:0:0:FB"); // node-local unicast
    pi_assert (!ip2.isWildcard());
    pi_assert (!ip2.isBroadcast());
    pi_assert (!ip2.isLoopback());
    pi_assert (ip2.isMulticast());
    pi_assert (!ip2.isUnicast());
    pi_assert (!ip2.isLinkLocal());
    pi_assert (!ip2.isSiteLocal());
    pi_assert (ip2.isWellKnownMC());
    pi_assert (ip2.isNodeLocalMC());
    pi_assert (!ip2.isLinkLocalMC());
    pi_assert (!ip2.isSiteLocalMC());
    pi_assert (!ip2.isOrgLocalMC());
    pi_assert (!ip2.isGlobalMC());

    IPAddress ip3("ff05:0:0:0:0:0:0:FB"); // site local unicast
    pi_assert (!ip3.isWildcard());
    pi_assert (!ip3.isBroadcast());
    pi_assert (!ip3.isLoopback());
    pi_assert (ip3.isMulticast());
    pi_assert (!ip3.isUnicast());
    pi_assert (!ip3.isLinkLocal());
    pi_assert (!ip3.isSiteLocal());
    pi_assert (ip3.isWellKnownMC());
    pi_assert (!ip3.isNodeLocalMC());
    pi_assert (!ip3.isLinkLocalMC());
    pi_assert (ip3.isSiteLocalMC());
    pi_assert (!ip3.isOrgLocalMC());
    pi_assert (!ip3.isGlobalMC());

    IPAddress ip4("ff18:0:0:0:0:0:0:FB"); // org local unicast
    pi_assert (!ip4.isWildcard());
    pi_assert (!ip4.isBroadcast());
    pi_assert (!ip4.isLoopback());
    pi_assert (ip4.isMulticast());
    pi_assert (!ip4.isUnicast());
    pi_assert (!ip4.isLinkLocal());
    pi_assert (!ip4.isSiteLocal());
    pi_assert (!ip4.isWellKnownMC());
    pi_assert (!ip4.isNodeLocalMC());
    pi_assert (!ip4.isLinkLocalMC());
    pi_assert (!ip4.isSiteLocalMC());
    pi_assert (ip4.isOrgLocalMC());
    pi_assert (!ip4.isGlobalMC());

    IPAddress ip5("ff1f:0:0:0:0:0:0:FB"); // global unicast
    pi_assert (!ip5.isWildcard());
    pi_assert (!ip5.isBroadcast());
    pi_assert (!ip5.isLoopback());
    pi_assert (ip5.isMulticast());
    pi_assert (!ip5.isUnicast());
    pi_assert (!ip5.isLinkLocal());
    pi_assert (!ip5.isSiteLocal());
    pi_assert (!ip5.isWellKnownMC());
    pi_assert (!ip5.isNodeLocalMC());
    pi_assert (!ip5.isLinkLocalMC());
    pi_assert (!ip5.isSiteLocalMC());
    pi_assert (!ip5.isOrgLocalMC());
    pi_assert (ip5.isGlobalMC());
#endif
}


void IPAddressTest::testRelationals()
{
    IPAddress ip1("192.168.1.120");
    IPAddress ip2(ip1);
    IPAddress ip3;
    IPAddress ip4("10.0.0.138");

    pi_assert (ip1 != ip4);
    pi_assert (ip1 == ip2);
    pi_assert (!(ip1 != ip2));
    pi_assert (!(ip1 == ip4));
    pi_assert (ip1 > ip4);
    pi_assert (ip1 >= ip4);
    pi_assert (ip4 < ip1);
    pi_assert (ip4 <= ip1);
    pi_assert (!(ip1 < ip4));
    pi_assert (!(ip1 <= ip4));
    pi_assert (!(ip4 > ip1));
    pi_assert (!(ip4 >= ip1));

    ip3 = ip1;
    pi_assert (ip1 == ip3);
    ip3 = ip4;
    pi_assert (ip1 != ip3);
    pi_assert (ip3 == ip4);
}


void IPAddressTest::testWildcard()
{
    IPAddress wildcard = IPAddress::wildcard();
    pi_assert (wildcard.isWildcard());
    pi_assert (wildcard.toString() == "0.0.0.0");
}


void IPAddressTest::testBroadcast()
{
    IPAddress broadcast = IPAddress::broadcast();
    pi_assert (broadcast.isBroadcast());
    pi_assert (broadcast.toString() == "255.255.255.255");
}


void IPAddressTest::testPrefixCons()
{
    IPAddress ia1(15, IPAddress::IPv4);
    pi_assert(ia1.toString() == "255.254.0.0");

#ifdef PIL_HAVE_IPv6
    IPAddress ia2(62, IPAddress::IPv6);
    pi_assert(ia2.toString() == "ffff:ffff:ffff:fffc::");
#endif
}


void IPAddressTest::testPrefixLen()
{
    IPAddress ia1(15, IPAddress::IPv4);
    pi_assert(ia1.prefixLength() == 15);

    IPAddress ia2(16, IPAddress::IPv4);
    pi_assert(ia2.prefixLength() == 16);

    IPAddress ia3(23, IPAddress::IPv4);
    pi_assert(ia3.prefixLength() == 23);

    IPAddress ia4(24, IPAddress::IPv4);
    pi_assert(ia4.prefixLength() == 24);

    IPAddress ia5(25, IPAddress::IPv4);
    pi_assert(ia5.prefixLength() == 25);

#ifdef PIL_HAVE_IPv6
    IPAddress ia6(62, IPAddress::IPv6);
    pi_assert(ia6.prefixLength() == 62);

    IPAddress ia7(63, IPAddress::IPv6);
    pi_assert(ia7.prefixLength() == 63);

    IPAddress ia8(64, IPAddress::IPv6);
    pi_assert(ia8.prefixLength() == 64);

    IPAddress ia9(65, IPAddress::IPv6);
    pi_assert(ia9.prefixLength() == 65);
#endif
}


void IPAddressTest::testOperators()
{
    IPAddress ip("10.0.0.51");
    IPAddress mask(24, IPAddress::IPv4);

    IPAddress net = ip & mask;
    pi_assert(net.toString() == "10.0.0.0");

    IPAddress host("0.0.0.51");
    pi_assert((net | host) == ip);

    pi_assert((~mask).toString() == "0.0.0.255");
}


void IPAddressTest::testRelationals6()
{
#ifdef PIL_HAVE_IPv6
#endif
}


void IPAddressTest::testByteOrderMacros()
{
    UInt16 a16 = 0xDEAD;
    pi_assert (pil_ntoh_16(a16) == ntohs(a16));
    pi_assert (pil_hton_16(a16) == htons(a16));
    UInt32 a32 = 0xDEADBEEF;
    pi_assert (pil_ntoh_32(a32) == ntohl(a32));
    pi_assert (pil_hton_32(a32) == htonl(a32));
}
