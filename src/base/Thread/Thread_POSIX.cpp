#include "../Environment.h"
#ifdef PIL_OS_FAMILY_UNIX

#include "Thread_POSIX.h"
#include "Thread.h"
#include "../Debug/Exception.h"
#include "../Time/Timestamp.h"
#include <signal.h>
#if defined(__sun) && defined(__SVR4)
#	if !defined(__EXTENSIONS__)
#		define __EXTENSIONS__
#	endif
#endif
#if PIL_OS == PIL_OS_LINUX || PIL_OS == PIL_OS_MAC_OS_X || PIL_OS == PIL_OS_QNX
#	include <time.h>
#endif

//
// Block SIGPIPE in main thread.
//
#if defined(PIL_OS_FAMILY_UNIX) && !defined(PIL_VXWORKS)
namespace
{
    class SignalBlocker
    {
    public:
        SignalBlocker()
        {
            sigset_t sset;
            sigemptyset(&sset);
            sigaddset(&sset, SIGPIPE);
            pthread_sigmask(SIG_BLOCK, &sset, 0);
        }
        ~SignalBlocker()
        {
        }
    };

    static SignalBlocker signalBlocker;
}
#endif


#if defined(PIL_POSIX_DEBUGGER_THREAD_NAMES)


namespace
{
    void setThreadName(pthread_t thread, const char* threadName)
    {
#   if (PIL_OS == PIL_OS_MAC_OS_X)
        pthread_setname_np(threadName); // __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2)
#   else
        pthread_setname_np(thread, threadName);
#   endif
    }
}


#endif


namespace pi {


ThreadImpl::CurrentThreadHolder ThreadImpl::_currentThreadHolder;


ThreadImpl::ThreadImpl():
    _pData(new ThreadData)
{
}


ThreadImpl::~ThreadImpl()
{
    if (_pData->started && !_pData->joined)
    {
        pthread_detach(_pData->thread);
    }
}


void ThreadImpl::setPriorityImpl(int prio)
{
    if (prio != _pData->prio)
    {
        _pData->prio = prio;
        _pData->policy = SCHED_OTHER;
        if (isRunningImpl())
        {
            struct sched_param par; struct MyStruct
            {

            };
            par.sched_priority = mapPrio(_pData->prio, SCHED_OTHER);
            if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par))
                throw SystemException("cannot set thread priority");
        }
    }
}


void ThreadImpl::setOSPriorityImpl(int prio, int policy)
{
    if (prio != _pData->osPrio || policy != _pData->policy)
    {
        if (_pData->pRunnableTarget)
        {
            struct sched_param par;
            par.sched_priority = prio;
            if (pthread_setschedparam(_pData->thread, policy, &par))
                throw SystemException("cannot set thread priority");
        }
        _pData->prio   = reverseMapPrio(prio, policy);
        _pData->osPrio = prio;
        _pData->policy = policy;
    }
}


int ThreadImpl::getMinOSPriorityImpl(int policy)
{
#if defined(PIL_THREAD_PRIORITY_MIN)
    return PIL_THREAD_PRIORITY_MIN;
#elif defined(__VMS) || defined(__digital__)
    return PRI_OTHER_MIN;
#else
    return sched_get_priority_min(policy);
#endif
}


int ThreadImpl::getMaxOSPriorityImpl(int policy)
{
#if defined(PIL_THREAD_PRIORITY_MAX)
    return PIL_THREAD_PRIORITY_MAX;
#elif defined(__VMS) || defined(__digital__)
    return PRI_OTHER_MAX;
#else
    return sched_get_priority_max(policy);
#endif
}


void ThreadImpl::setStackSizeImpl(int size)
{
#ifndef PTHREAD_STACK_MIN
    _pData->stackSize = 0;
#else
    if (size != 0)
    {
#if defined(PIL_OS_FAMILY_BSD)
        // we must round up to a multiple of the memory page size
        const int STACK_PAGE_SIZE = 4096;
        size = ((size + STACK_PAGE_SIZE - 1)/STACK_PAGE_SIZE)*STACK_PAGE_SIZE;
#endif
#if !defined(PIL_ANDROID)
        if (size < PTHREAD_STACK_MIN)
            size = PTHREAD_STACK_MIN;
#endif
    }
    _pData->stackSize = size;
#endif
}


void ThreadImpl::startImpl(Runnable* pTarget)
{
    if (_pData->pRunnableTarget)
        throw SystemException("thread already running");

    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    if (_pData->stackSize != 0)
    {
        if (0 != pthread_attr_setstacksize(&attributes, _pData->stackSize))
        {
            pthread_attr_destroy(&attributes);
            throw SystemException("cannot set thread stack size");
        }
    }

    _pData->pRunnableTarget = pTarget;
    if (pthread_create(&_pData->thread, &attributes, runnableEntry, this))
    {
        _pData->pRunnableTarget = 0;
        pthread_attr_destroy(&attributes);
        throw SystemException("cannot start thread");
    }
    _pData->started = true;
    pthread_attr_destroy(&attributes);

    if (_pData->policy == SCHED_OTHER)
    {
        if (_pData->prio != PRIO_NORMAL_IMPL)
        {
            struct sched_param par;
            par.sched_priority = mapPrio(_pData->prio, SCHED_OTHER);
            if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par))
                throw SystemException("cannot set thread priority");
        }
    }
    else
    {
        struct sched_param par;
        par.sched_priority = _pData->osPrio;
        if (pthread_setschedparam(_pData->thread, _pData->policy, &par))
            throw SystemException("cannot set thread priority");
    }
}


void ThreadImpl::joinImpl()
{
    if (!_pData->started) return;
    _pData->done.wait();
    void* result;
    if (pthread_join(_pData->thread, &result))
        throw SystemException("cannot join thread");
    _pData->joined = true;
}


bool ThreadImpl::joinImpl(long milliseconds)
{
    if (_pData->started && _pData->done.tryWait(milliseconds))
    {
        void* result;
        if (pthread_join(_pData->thread, &result))
            throw SystemException("cannot join thread");
        _pData->joined = true;
        return true;
    }
    else if (_pData->started) return false;
    else return true;
}


ThreadImpl* ThreadImpl::currentImpl()
{
    return _currentThreadHolder.get();
}


ThreadImpl::TIDImpl ThreadImpl::currentTidImpl()
{
    return pthread_self();
}


void ThreadImpl::sleepImpl(long milliseconds)
{
    pi::sleep(milliseconds*1000);
}


void* ThreadImpl::runnableEntry(void* pThread)
{
    _currentThreadHolder.set(reinterpret_cast<ThreadImpl*>(pThread));

#if defined(PIL_OS_FAMILY_UNIX)
    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGQUIT);
    sigaddset(&sset, SIGTERM);
    sigaddset(&sset, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &sset, 0);
#endif

    ThreadImpl* pThreadImpl = reinterpret_cast<ThreadImpl*>(pThread);
#if defined(PIL_POSIX_DEBUGGER_THREAD_NAMES)
    setThreadName(pThreadImpl->_pData->thread, reinterpret_cast<Thread*>(pThread)->getName().c_str());
#endif
    SPtr<ThreadData> pData = pThreadImpl->_pData;
    try
    {
        pData->pRunnableTarget->run();
    }
    catch (Exception& exc)
    {
//        ErrorHandler::handle(exc);
    }
    catch (std::exception& exc)
    {
//        ErrorHandler::handle(exc);
    }
    catch (...)
    {
//        ErrorHandler::handle();
    }

    pData->pRunnableTarget = 0;
    pData->done.set();
    return 0;
}


int ThreadImpl::mapPrio(int prio, int policy)
{
    int pmin = getMinOSPriorityImpl(policy);
    int pmax = getMaxOSPriorityImpl(policy);

    switch (prio)
    {
    case PRIO_LOWEST_IMPL:
        return pmin;
    case PRIO_LOW_IMPL:
        return pmin + (pmax - pmin)/4;
    case PRIO_NORMAL_IMPL:
        return pmin + (pmax - pmin)/2;
    case PRIO_HIGH_IMPL:
        return pmin + 3*(pmax - pmin)/4;
    case PRIO_HIGHEST_IMPL:
        return pmax;
    default:
        return pmin + (pmax - pmin)/2;
    }
    return -1; // just to satisfy compiler - we'll never get here anyway
}


int ThreadImpl::reverseMapPrio(int prio, int policy)
{
    if (policy == SCHED_OTHER)
    {
        int pmin = getMinOSPriorityImpl(policy);
        int pmax = getMaxOSPriorityImpl(policy);
        int normal = pmin + (pmax - pmin)/2;
        if (prio == pmax)
            return PRIO_HIGHEST_IMPL;
        if (prio > normal)
            return PRIO_HIGH_IMPL;
        else if (prio == normal)
            return PRIO_NORMAL_IMPL;
        else if (prio > pmin)
            return PRIO_LOW_IMPL;
        else
            return PRIO_LOWEST_IMPL;
    }
    else return PRIO_HIGHEST_IMPL;
}


} // namespace PIL
#endif
