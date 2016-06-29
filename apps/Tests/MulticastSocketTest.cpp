#include <base/Utils/TestCase.h>
#include <network/MulticastEchoServer.h>

using namespace pi;
using namespace std;

class MulticastSocketTest: TestCase
{
public:
    MulticastSocketTest():TestCase("MulticastSocketTest"){}
    virtual void run()
    {
        testMulticast();
    }

    void testMulticast();

};
MulticastSocketTest MulticastSocketTestInstance;

void MulticastSocketTest::testMulticast()
{
    MulticastEchoServer echoServer;
    MulticastSocket ms;
    int n = ms.sendTo("hello", 5, echoServer.group());
    pi_assert (n == 5);
    char buffer[256];
    n = ms.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ms.close();
}
