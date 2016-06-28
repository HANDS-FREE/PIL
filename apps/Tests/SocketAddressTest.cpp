#include <network/SocketAddress.h>
#include <network/NetException.h>
#include <base/Utils/TestCase.h>

using namespace std;
using namespace pi;

class SocketAddressTest : public TestCase
{
public:
    SocketAddressTest():TestCase("SocketAddressTest"){}

    virtual void run()
    {
        testSocketAddress();
        testSocketRelationals();
        testSocketAddress6();
    }

    void testSocketAddress();
    void testSocketRelationals();
    void testSocketAddress6();
};

SocketAddressTest socketAddressTest;

void SocketAddressTest::testSocketAddress()
{
    SocketAddress wild;
    pi_assert (wild.host().isWildcard());
    pi_assert (wild.port() == 0);

    SocketAddress sa1("192.168.1.100", 100);
    pi_assert (sa1.host().toString() == "192.168.1.100");
    pi_assert (sa1.port() == 100);

    SocketAddress sa2("192.168.1.100", "100");
    pi_assert (sa2.host().toString() == "192.168.1.100");
    pi_assert (sa2.port() == 100);

#if !defined(_WIN32_WCE)
    SocketAddress sa3("192.168.1.100", "ftp");
    pi_assert (sa3.host().toString() == "192.168.1.100");
    pi_assert (sa3.port() == 21);
#endif

    try
    {
        SocketAddress sa3("192.168.1.100", "f00bar");
        fail("bad service name - must throw");
    }
    catch (ServiceNotFoundException&)
    {
    }

    SocketAddress sa4("www.appinf.com", 80);
    pi_assert (sa4.host().toString() == "162.209.7.4");
    pi_assert (sa4.port() == 80);

    try
    {
        SocketAddress sa5("192.168.2.260", 80);
        fail("invalid address - must throw");
    }
    catch (HostNotFoundException&)
    {
    }
    catch (NoAddressFoundException&)
    {
    }

    try
    {
        SocketAddress sa6("192.168.2.120", "80000");
        fail("invalid port - must throw");
    }
    catch (ServiceNotFoundException&)
    {
    }

    SocketAddress sa7("192.168.2.120:88");
    pi_assert (sa7.host().toString() == "192.168.2.120");
    pi_assert (sa7.port() == 88);

    SocketAddress sa8("[192.168.2.120]:88");
    pi_assert (sa8.host().toString() == "192.168.2.120");
    pi_assert (sa8.port() == 88);

    try
    {
        SocketAddress sa9("[192.168.2.260]");
        fail("invalid address - must throw");
    }
    catch (InvalidArgumentException&)
    {
    }

    try
    {
        SocketAddress sa9("[192.168.2.260:88");
        fail("invalid address - must throw");
    }
    catch (InvalidArgumentException&)
    {
    }
}


void SocketAddressTest::testSocketRelationals()
{
    SocketAddress sa1("192.168.1.100", 100);
    SocketAddress sa2("192.168.1.100:100");
    pi_assert (sa1 == sa2);

    SocketAddress sa3("192.168.1.101", "99");
    pi_assert (sa2 < sa3);

    SocketAddress sa4("192.168.1.101", "102");
    pi_assert (sa3 < sa4);
}


void SocketAddressTest::testSocketAddress6()
{
#ifdef POCO_HAVE_IPv6
#endif
}
