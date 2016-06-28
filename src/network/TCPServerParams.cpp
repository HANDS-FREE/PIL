#include "TCPServerParams.h"


namespace pi {


TCPServerParams::TCPServerParams():
    _threadIdleTime(10000000),
    _maxThreads(0),
    _maxQueued(64),
    _threadPriority(pi::Thread::PRIO_NORMAL)
{
}


TCPServerParams::~TCPServerParams()
{
}


void TCPServerParams::setThreadIdleTime(const pi::Timespan& milliseconds)
{
    _threadIdleTime = milliseconds;
}


void TCPServerParams::setMaxThreads(int count)
{
    pi_assert (count > 0);

    _maxThreads = count;
}


void TCPServerParams::setMaxQueued(int count)
{
    pi_assert (count >= 0);

    _maxQueued = count;
}


void TCPServerParams::setThreadPriority(pi::Thread::Priority prio)
{
    _threadPriority = prio;
}


} // namespace pi
