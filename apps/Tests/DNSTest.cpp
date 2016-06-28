#include <network/DNS.h>
#include <network/NetException.h>
#include <base/Utils/TestCase.h>

using namespace std;
using namespace pi;

class DNSTest : public TestCase
{
  public:
    DNSTest():TestCase("DNSTest"){}
    virtual void run()
    {
        testHostByName();
        testHostByAddress();
    }
    void testHostByName();
    void testHostByAddress();
};
DNSTest DNSTestinstance;

void DNSTest::testHostByName()
{
    HostEntry he1 = DNS::hostByName("aliastest.appinf.com");
    // different systems report different canonical names, unfortunately.
    pi_assert (he1.name() == "dnstest.appinf.com" || he1.name() == "aliastest.appinf.com");
#if !defined(PIL_HAVE_ADDRINFO)
    // getaddrinfo() does not report any aliases
    pi_assert (!he1.aliases().empty());
    pi_assert (he1.aliases()[0] == "aliastest.appinf.com");
#endif
    pi_assert (he1.addresses().size() >= 1);
    pi_assert (he1.addresses()[0].toString() == "1.2.3.4");

    try
    {
        HostEntry he1 = DNS::hostByName("nohost.appinf.com");
        fail("host not found - must throw");
    }
    catch (HostNotFoundException&)
    {
    }
    catch (NoAddressFoundException&)
    {
    }
}


void DNSTest::testHostByAddress()
{
    IPAddress ip1("80.122.195.86");
    HostEntry he1 = DNS::hostByAddress(ip1);
    pi_assert (he1.name() == "mailhost.appinf.com");
    pi_assert (he1.aliases().empty());
    pi_assert (he1.addresses().size() >= 1);
    pi_assert (he1.addresses()[0].toString() == "80.122.195.86");

    IPAddress ip2("10.0.244.253");
    try
    {
        HostEntry he2 = DNS::hostByAddress(ip2);
        fail("host not found - must throw");
    }
    catch (HostNotFoundException&)
    {
    }
    catch (NoAddressFoundException&)
    {
    }
}

