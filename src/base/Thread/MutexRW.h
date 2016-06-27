#ifndef PIL_MUTEXRW_H
#define PIL_MUTEXRW_H

#include "../Environment.h"
#include "../Debug/Exception.h"

#if defined(PIL_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "MutexRW_WINCE.h"
#else
#include "MutexRW_WIN32.h"
#endif
#elif defined(PIL_ANDROID)
#include "MutexRW_Android.h"
#elif defined(PIL_VXWORKS)
#include "MutexRW_VX.h"
#else
#include "MutexRW_POSIX.h"
#endif

#include "ScopedLock.h"

namespace pi{

class PIL_API MutexRW: private MutexRWImpl
    /// A reader writer lock allows multiple concurrent
    /// readers or one exclusive writer.
{
public:
    MutexRW(){}
        /// Creates the Reader/Writer lock.

    ~MutexRW(){}
        /// Destroys the Reader/Writer lock.

    void lock(){writeLock();}
        /// Default is write lock

    void readLock(){readLockImpl();}
        /// Acquires a read lock. If another thread currently holds a write lock,
        /// waits until the write lock is released.

    bool tryReadLock(){return tryReadLockImpl();}
        /// Tries to acquire a read lock. Immediately returns true if successful, or
        /// false if another thread currently holds a write lock.

    void writeLock(){return writeLockImpl();}
        /// Acquires a write lock. If one or more other threads currently hold
        /// locks, waits until all locks are released. The results are undefined
        /// if the same thread already holds a read or write lock

    bool tryWriteLock(){return tryWriteLockImpl();}
        /// Tries to acquire a write lock. Immediately returns true if successful,
        /// or false if one or more other threads currently hold
        /// locks. The result is undefined if the same thread already
        /// holds a read or write lock.

    void unlock(){return unlockImpl();}
        /// Releases the read or write lock.

private:
    MutexRW(const MutexRW&);
    MutexRW& operator = (const MutexRW&);
};

typedef WriteLock<MutexRW> WriteMutex;
typedef ReadLock<MutexRW>  ReadMutex;

}
#endif // MUTEXRW_H
