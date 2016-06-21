#include "../Environment.h"
#ifdef PIL_OS_FAMILY_UNIX
#ifndef PIL_Thread_POSIX_INCLUDED
#define PIL_Thread_POSIX_INCLUDED

#define PIL_THREAD_STACK_SIZE 0

// must be limits.h (not <climits>) for PTHREAD_STACK_MIN on Solaris
#include <limits.h>
#if !defined(PIL_NO_SYS_SELECT_H)
#include <sys/select.h>
#endif
#include <errno.h>
#if defined(PIL_VXWORKS)
#include <cstring>
#endif

#include <pthread.h>

#include "../Environment.h"
#include "../Types/SPtr.h"

#include "Runnable.h"
#include "Mutex.h"
#include "Event.h"


namespace pi {


class PIL_API ThreadImpl
{
public:
    typedef pthread_t TIDImpl;
    typedef void (*Callable)(void*);

    enum Priority
    {
        PRIO_LOWEST_IMPL,
        PRIO_LOW_IMPL,
        PRIO_NORMAL_IMPL,
        PRIO_HIGH_IMPL,
        PRIO_HIGHEST_IMPL
    };

    enum Policy
    {
        POLICY_DEFAULT_IMPL = SCHED_OTHER
    };

    ThreadImpl();
    ~ThreadImpl();

    TIDImpl tidImpl() const;
    void setPriorityImpl(int prio);
    int getPriorityImpl() const;
    void setOSPriorityImpl(int prio, int policy = SCHED_OTHER);
    int getOSPriorityImpl() const;
    static int getMinOSPriorityImpl(int policy);
    static int getMaxOSPriorityImpl(int policy);
    void setStackSizeImpl(int size);
    int getStackSizeImpl() const;
    void startImpl(Runnable* pTarget);
    void joinImpl();
    bool joinImpl(long milliseconds);
    bool isRunningImpl() const;
    static void sleepImpl(long milliseconds);
    static void yieldImpl();
    static ThreadImpl* currentImpl();
    static TIDImpl currentTidImpl();

protected:
    static void* runnableEntry(void* pThread);
    static int mapPrio(int prio, int policy = SCHED_OTHER);
    static int reverseMapPrio(int osPrio, int policy = SCHED_OTHER);

private:
    class CurrentThreadHolder
    {
    public:
        CurrentThreadHolder()
        {
            if (pthread_key_create(&_key, NULL))
                throw SystemException("cannot allocate thread context key");
        }
        ~CurrentThreadHolder()
        {
            pthread_key_delete(_key);
        }
        ThreadImpl* get() const
        {
            return reinterpret_cast<ThreadImpl*>(pthread_getspecific(_key));
        }
        void set(ThreadImpl* pThread)
        {
            pthread_setspecific(_key, pThread);
        }

    private:
        pthread_key_t _key;
    };

    struct ThreadData
    {
        ThreadData():
            thread(0),
            prio(PRIO_NORMAL_IMPL),
            policy(SCHED_OTHER),
            done(false),
            stackSize(PIL_THREAD_STACK_SIZE),
            started(false),
            joined(false)
        {
        #if defined(PIL_VXWORKS)
            // This workaround is for VxWorks 5.x where
            // pthread_init() won't properly initialize the thread.
            std::memset(&thread, 0, sizeof(thread));
        #endif
        }

        Runnable*     pRunnableTarget;
        pthread_t     thread;
        int           prio;
        int           osPrio;
        int           policy;
        Event         done;
        std::size_t   stackSize;
        bool          started;
        bool          joined;
    };

    SPtr<ThreadData> _pData;

    static CurrentThreadHolder _currentThreadHolder;
};


//
// inlines
//
inline int ThreadImpl::getPriorityImpl() const
{
    return _pData->prio;
}


inline int ThreadImpl::getOSPriorityImpl() const
{
    return _pData->osPrio;
}


inline bool ThreadImpl::isRunningImpl() const
{
    return _pData->pRunnableTarget;
}


inline void ThreadImpl::yieldImpl()
{
    sched_yield();
}


inline int ThreadImpl::getStackSizeImpl() const
{
    return static_cast<int>(_pData->stackSize);
}


inline ThreadImpl::TIDImpl ThreadImpl::tidImpl() const
{
    return _pData->thread;
}


} // namespace PIL


#endif // PIL_Thread_POSIX_INCLUDED
#endif // PIL_OS_FAMILY_UNIX
