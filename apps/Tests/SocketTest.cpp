#include <base/Time/Global_Timer.h>
#include <base/Thread/Delegate.h>
#include <network/EchoServer.h>
#include <network/StreamSocket.h>
#include <network/SocketAddress.h>
#include <network/ServerSocket.h>
#include <network/NetException.h>
#include <base/Utils/TestCase.h>

using namespace pi;
using namespace std;

class SocketTest : public pi::TestCase
{
public:
    SocketTest():pi::TestCase("SocketTest"){setUp();}

    virtual void run()
    {
        testEcho();
        testPoll();
        testAvailable();
        testFIFOBuffer();
        testConnect();
        testConnectRefused();
        testConnectRefusedNB();
        testNonBlocking();
        testAddress();
        testAssign();
        testTimeout();
        testBufferSize();
        testOptions();
        testSelect();
        testSelect2();
        testSelect3();
    }

    void testEcho();
    void testPoll();
    void testAvailable();
    void testFIFOBuffer();
    void testConnect();
    void testConnectRefused();
    void testConnectRefusedNB();
    void testNonBlocking();
    void testAddress();
    void testAssign();
    void testTimeout();
    void testBufferSize();
    void testOptions();
    void testSelect();
    void testSelect2();
    void testSelect3();

    void setUp();
    void tearDown();


private:
    void onReadable(bool& b);
    void onWritable(bool& b);

    int _readableToNot;
    int _notToReadable;
    int _writableToNot;
    int _notToWritable;
};

void SocketTest::testEcho()
{
    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));
    int n = ss.sendBytes("hello", 5);
    pi_assert (n == 5);
    char buffer[256];
    n = ss.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}

void SocketTest::testPoll()
{
    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));
    pi::TicTac sw;
    Timespan timeout(1000000);
    pi_assert (!ss.poll(timeout, Socket::SELECT_READ));
    pi_assert (sw.Tac() >= 1);
    sw.Tic();
    pi_assert (ss.poll(timeout, Socket::SELECT_WRITE));
    pi_assert (sw.Tac() < 100000);
    ss.sendBytes("hello", 5);
    char buffer[256];
    sw.Tic();
    pi_assert (ss.poll(timeout, Socket::SELECT_READ));
    pi_assert (sw.Tac() < 100000);
    int n = ss.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}


void SocketTest::testAvailable()
{
    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));
    Timespan timeout(1000000);
    ss.sendBytes("hello", 5);
    char buffer[256];
    pi_assert (ss.poll(timeout, Socket::SELECT_READ));
    int av = ss.available();
    pi_assert (av > 0 && av <= 5);
    int n = ss.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}


void SocketTest::testFIFOBuffer()
{
    Buffer<char> b(5);
    b[0] = 'h';
    b[1] = 'e';
    b[2] = 'l';
    b[3] = 'l';
    b[4] = 'o';

    FIFOBuffer f(5, true);

    f.readable += delegate(this, &SocketTest::onReadable);
    f.writable += delegate(this, &SocketTest::onWritable);

    pi_assert(0 == _notToReadable);
    pi_assert(0 == _readableToNot);
    pi_assert(0 == _notToWritable);
    pi_assert(0 == _writableToNot);
    f.write(b);
    pi_assert(1 == _notToReadable);
    pi_assert(0 == _readableToNot);
    pi_assert(0 == _notToWritable);
    pi_assert(1 == _writableToNot);

    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));
    int n = ss.sendBytes(f);
    pi_assert (n == 5);
    pi_assert(1 == _notToReadable);
    pi_assert(1 == _readableToNot);
    pi_assert(1 == _notToWritable);
    pi_assert(1 == _writableToNot);
    pi_assert (f.isEmpty());

    n = ss.receiveBytes(f);
    pi_assert (n == 5);

    pi_assert(2 == _notToReadable);
    pi_assert(1 == _readableToNot);
    pi_assert(1 == _notToWritable);
    pi_assert(2 == _writableToNot);

    pi_assert (f[0] == 'h');
    pi_assert (f[1] == 'e');
    pi_assert (f[2] == 'l');
    pi_assert (f[3] == 'l');
    pi_assert (f[4] == 'o');

    f.readable -= delegate(this, &SocketTest::onReadable);
    f.writable -= delegate(this, &SocketTest::onWritable);

    ss.close();
}


void SocketTest::testConnect()
{
    ServerSocket serv;
    serv.bind(SocketAddress());
    serv.listen();
    StreamSocket ss;
    Timespan timeout(250000);
    ss.connect(SocketAddress("localhost", serv.address().port()), timeout);
}


void SocketTest::testConnectRefused()
{
    ServerSocket serv;
    serv.bind(SocketAddress());
    serv.listen();
    UInt16 port = serv.address().port();
    serv.close();
    StreamSocket ss;
    try
    {
        ss.connect(SocketAddress("localhost", port));
        pi_assert2(true,("connection refused - must throw"));
    }
    catch (ConnectionRefusedException&)
    {
    }
}


void SocketTest::testConnectRefusedNB()
{
    ServerSocket serv;
    serv.bind(SocketAddress());
    serv.listen();
    UInt16 port = serv.address().port();
    serv.close();
    StreamSocket ss;
    Timespan timeout(2, 0);
    try
    {
        ss.connect(SocketAddress("localhost", port), timeout);
        pi_assert2(false,("connection refused - must throw"));
    }
    catch (TimeoutException&)
    {
    }
    catch (ConnectionRefusedException&)
    {
    }
}


void SocketTest::testNonBlocking()
{
    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));
    ss.setBlocking(false);

    Timespan timeout(1000000);
    pi_assert (ss.poll(timeout, Socket::SELECT_WRITE));
    int n = ss.sendBytes("hello", 5);
    pi_assert (n == 5);

    char buffer[256];
    pi_assert (ss.poll(timeout, Socket::SELECT_READ));
    n = ss.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}



void SocketTest::testAddress()
{
    ServerSocket serv;
    serv.bind(SocketAddress());
    serv.listen();
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", serv.address().port()));
    StreamSocket css = serv.acceptConnection();
    pi_assert (css.peerAddress().host() == ss.address().host());
    pi_assert (css.peerAddress().port() == ss.address().port());
}


void SocketTest::testAssign()
{
    ServerSocket serv;
    StreamSocket ss1;
    StreamSocket ss2;

    pi_assert (ss1 != ss2);
    StreamSocket ss3(ss1);
    pi_assert (ss1 == ss3);
    ss3 = ss2;
    pi_assert (ss1 != ss3);
    pi_assert (ss2 == ss3);

    try
    {
        ss1 = serv;
        pi_assert2(true,("incompatible assignment - must throw"));
    }
    catch (InvalidArgumentException&)
    {
    }

    try
    {
        StreamSocket ss4(serv);
        pi_assert2(true,("incompatible assignment - must throw"));
    }
    catch (InvalidArgumentException&)
    {
    }

    try
    {
        serv = ss1;
        pi_assert2(true,("incompatible assignment - must throw"));
    }
    catch (InvalidArgumentException&)
    {
    }

    try
    {
        ServerSocket serv2(ss1);
        pi_assert2(true,("incompatible assignment - must throw"));
    }
    catch (InvalidArgumentException&)
    {
    }
}


void SocketTest::testTimeout()
{
    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));

    Timespan timeout0 = ss.getReceiveTimeout();
    Timespan timeout(250000);
    ss.setReceiveTimeout(timeout);
    Timespan timeout1 = ss.getReceiveTimeout();
    std::cout << "original receive timeout:  " << timeout0.totalMicroseconds() << std::endl;
    std::cout << "requested receive timeout: " << timeout.totalMicroseconds() << std::endl;
    std::cout << "actual receive timeout:    " << timeout1.totalMicroseconds() << std::endl;

    // some socket implementations adjust the timeout value
    // assert (ss.getReceiveTimeout() == timeout);
    TicTac sw;
    try
    {
        char buffer[256];
        sw.Tic();
        ss.receiveBytes(buffer, sizeof(buffer));
        pi_assert2(true,"nothing to receive - must timeout");
    }
    catch (TimeoutException&)
    {
    }
    pi_assert (sw.Tac() < 1.);

    timeout0 = ss.getSendTimeout();
    ss.setSendTimeout(timeout);
    timeout1 = ss.getSendTimeout();
    std::cout << "original send timeout:  " << timeout0.totalMicroseconds() << std::endl;
    std::cout << "requested send timeout: " << timeout.totalMicroseconds() << std::endl;
    std::cout << "actual send timeout:    " << timeout1.totalMicroseconds() << std::endl;
    // assert (ss.getSendTimeout() == timeout);
}


void SocketTest::testBufferSize()
{
    EchoServer echoServer;
    SocketAddress sa("localhost", 1234);
    StreamSocket ss(sa.family());

    int osz = ss.getSendBufferSize();
    int rsz = 32000;
    ss.setSendBufferSize(rsz);
    int asz = ss.getSendBufferSize();
    std::cout << "original send buffer size:  " << osz << std::endl;
    std::cout << "requested send buffer size: " << rsz << std::endl;
    std::cout << "actual send buffer size:    " << asz << std::endl;

    osz = ss.getReceiveBufferSize();
    ss.setReceiveBufferSize(rsz);
    asz = ss.getReceiveBufferSize();
    std::cout << "original recv buffer size:  " << osz << std::endl;
    std::cout << "requested recv buffer size: " << rsz << std::endl;
    std::cout << "actual recv buffer size:    " << asz << std::endl;
}


void SocketTest::testOptions()
{
    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));

    ss.setLinger(true, 20);
    bool f;
    int  t;
    ss.getLinger(f, t);
    pi_assert (f && t == 20);
    ss.setLinger(false, 0);
    ss.getLinger(f, t);
    pi_assert (!f);

    ss.setNoDelay(true);
    pi_assert (ss.getNoDelay());
    ss.setNoDelay(false);
    pi_assert (!ss.getNoDelay());

    ss.setKeepAlive(true);
    pi_assert (ss.getKeepAlive());
    ss.setKeepAlive(false);
    pi_assert (!ss.getKeepAlive());

    ss.setOOBInline(true);
    pi_assert (ss.getOOBInline());
    ss.setOOBInline(false);
    pi_assert (!ss.getOOBInline());
}


void SocketTest::testSelect()
{
    Timespan timeout(250000);

    EchoServer echoServer;
    StreamSocket ss;
    ss.connect(SocketAddress("localhost", echoServer.port()));

    Socket::SocketList readList;
    Socket::SocketList writeList;
    Socket::SocketList exceptList;

    readList.push_back(ss);
    pi_assert (Socket::select(readList, writeList, exceptList, timeout) == 0);
    pi_assert (readList.empty());
    pi_assert (writeList.empty());
    pi_assert (exceptList.empty());

    ss.sendBytes("hello", 5);

    ss.poll(timeout, Socket::SELECT_READ);

    readList.push_back(ss);
    writeList.push_back(ss);
    pi_assert (Socket::select(readList, writeList, exceptList, timeout) == 2);
    pi_assert (!readList.empty());
    pi_assert (!writeList.empty());
    pi_assert (exceptList.empty());

    char buffer[256];
    int n = ss.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);
    pi_assert (std::string(buffer, n) == "hello");
    ss.close();
}


void SocketTest::testSelect2()
{
    Timespan timeout(100000);

    EchoServer echoServer1;
    EchoServer echoServer2;
    StreamSocket ss1(SocketAddress("localhost", echoServer1.port()));
    StreamSocket ss2(SocketAddress("localhost", echoServer2.port()));

    Socket::SocketList readList;
    Socket::SocketList writeList;
    Socket::SocketList exceptList;

    readList.push_back(ss1);
    readList.push_back(ss2);
    pi_assert (Socket::select(readList, writeList, exceptList, timeout) == 0);
    pi_assert (readList.empty());
    pi_assert (writeList.empty());
    pi_assert (exceptList.empty());

    ss1.sendBytes("hello", 5);

    ss1.poll(timeout, Socket::SELECT_READ);

    readList.push_back(ss1);
    readList.push_back(ss2);
    pi_assert (Socket::select(readList, writeList, exceptList, timeout) == 1);

    pi_assert (readList.size() == 1);
    pi_assert (readList[0] == ss1);
    pi_assert (writeList.empty());
    pi_assert (exceptList.empty());

    char buffer[256];
    int n = ss1.receiveBytes(buffer, sizeof(buffer));
    pi_assert (n == 5);

    readList.clear();
    writeList.clear();
    exceptList.clear();
    writeList.push_back(ss1);
    writeList.push_back(ss2);
    pi_assert (Socket::select(readList, writeList, exceptList, timeout) == 2);
    pi_assert (readList.empty());
    pi_assert (writeList.size() == 2);
    pi_assert (writeList[0] == ss1);
    pi_assert (writeList[1] == ss2);
    pi_assert (exceptList.empty());

    ss1.close();
    ss2.close();
}


void SocketTest::testSelect3()
{
    Socket::SocketList readList;
    Socket::SocketList writeList;
    Socket::SocketList exceptList;
    Timespan timeout(1000);

    int rc = Socket::select(readList, writeList, exceptList, timeout);
    pi_assert (rc == 0);
}


void SocketTest::onReadable(bool& b)
{
    if (b) ++_notToReadable;
    else ++_readableToNot;
}


void SocketTest::onWritable(bool& b)
{
    if (b) ++_notToWritable;
    else ++_writableToNot;
}


void SocketTest::setUp()
{
    _readableToNot = 0;
    _notToReadable = 0;
    _writableToNot = 0;
    _notToWritable = 0;
}


void SocketTest::tearDown()
{
}

SocketTest socketTest;
