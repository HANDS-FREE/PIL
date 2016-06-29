#include <network/DatagramSocket.h>
#include <network/UDPEchoServer.h>
#include <network/NetException.h>
#include <base/Utils/TestCase.h>
using namespace pi;

class DatagramSocketTest: public TestCase
{
public:
    DatagramSocketTest():TestCase("DatagramSocketTest"){}

    virtual void run()
    {
        testEcho();
        testSendToReceiveFrom();
        testBroadcast();
    }

    void testEcho();
    void testSendToReceiveFrom();
    void testBroadcast();

};
DatagramSocketTest DatagramSocketTestInstance;

void DatagramSocketTest::testEcho()
{
    UDPEchoServer echoServer;
    DatagramSocket ss;
    char buffer[256];
    ss.connect(SocketAddress("localhost", echoServer.port()));
    int n = ss.sendBytes("hello", 5);
    pi_assert (n == 5);
    n = ss.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}


void DatagramSocketTest::testSendToReceiveFrom()
{
    UDPEchoServer echoServer(SocketAddress("localhost", 0));
    DatagramSocket ss;
    int n = ss.sendTo("hello", 5, SocketAddress("localhost", echoServer.port()));
    pi_assert (n == 5);
    char buffer[256];
    SocketAddress sa;
    n = ss.receiveFrom(buffer, sizeof(buffer), sa);
    pi_assert (sa.host() == echoServer.address().host());
    pi_assert (sa.port() == echoServer.port());
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}


void DatagramSocketTest::testBroadcast()
{
    UDPEchoServer echoServer;
    DatagramSocket ss(IPAddress::IPv4);

#if defined(PIL_NET_HAS_INTERFACE) && (PIL_OS == PIL_OS_FREE_BSD)
    NetworkInterface ni = NetworkInterface::forName("em0");
    SocketAddress sa(ni.broadcastAddress(1), echoServer.port());
#else
    SocketAddress sa("255.255.255.255", echoServer.port());
#endif

    ss.setBroadcast(true);

#if (PIL_OS == PIL_OS_FREE_BSD)
    int opt = 1;
    pil_socklen_t len = sizeof(opt);
    ss.impl()->setRawOption(IPPROTO_IP, IP_ONESBCAST, (const char*) &opt, len);
    ss.impl()->getRawOption(IPPROTO_IP, IP_ONESBCAST, &opt, len);
    pi_assert (opt == 1);
#endif

    int n = ss.sendTo("hello", 5, sa);
    pi_assert (n == 5);
    char buffer[256] = { 0 };
    n = ss.receiveBytes(buffer, 5);
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}
