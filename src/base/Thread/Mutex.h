#ifndef PIL_Mutex_INCLUDED
#define PIL_Mutex_INCLUDED

#include "../Environment.h"
#include "../Debug/Exception.h"
#include "ScopedLock.h"


#if defined(PIL_OS_FAMILY_WINDOWS)
#if defined(_WIN32_WCE)
#include "Mutex_WINCE.h"
#else
#include "Mutex_WIN32.h"
#endif
#elif defined(PIL_VXWORKS)
#include "Mutex_VX.h"
#else
#include "Mutex_POSIX.h"
#endif


namespace pi {


class PIL_API Mutex: private MutexImpl
    /// A Mutex (mutual exclusion) is a synchronization
    /// mechanism used to control access to a shared resource
    /// in a concurrent (multithreaded) scenario.
    /// Mutexes are recursive, that is, the same mutex can be
    /// locked multiple times by the same thread (but, of course,
    /// not by other threads).
    /// Using the ScopedLock class is the preferred way to automatically
    /// lock and unlock a mutex.
{
public:
    typedef pi::ScopedLock<Mutex> ScopedLock;

    Mutex(){}
        /// creates the Mutex.

    ~Mutex(){}
        /// destroys the Mutex.

    void lock();
        /// Locks the mutex. Blocks if the mutex
        /// is held by another thread.

    void lock(long milliseconds);
        /// Locks the mutex. Blocks up to the given number of milliseconds
        /// if the mutex is held by another thread. Throws a TimeoutException
        /// if the mutex can not be locked within the given timeout.
        ///
        /// Performance Note: On most platforms (including Windows), this member function is
        /// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
        /// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

    bool tryLock();
        /// Tries to lock the mutex. Returns false immediately
        /// if the mutex is already held by another thread.
        /// Returns true if the mutex was successfully locked.

    bool tryLock(long milliseconds);
        /// Locks the mutex. Blocks up to the given number of milliseconds
        /// if the mutex is held by another thread.
        /// Returns true if the mutex was successfully locked.
        ///
        /// Performance Note: On most platforms (including Windows), this member function is
        /// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
        /// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

    void unlock();
        /// Unlocks the mutex so that it can be acquired by
        /// other threads.

private:
    Mutex(const Mutex&);
    Mutex& operator = (const Mutex&);
};


class PIL_API FastMutex: private FastMutexImpl
    /// A FastMutex (mutual exclusion) is similar to a Mutex.
    /// Unlike a Mutex, however, a FastMutex is not recursive,
    /// which means that a deadlock will occur if the same
    /// thread tries to lock a mutex it has already locked again.
    /// Locking a FastMutex is faster than locking a recursive Mutex.
    /// Using the ScopedLock class is the preferred way to automatically
    /// lock and unlock a mutex.
{
public:
    typedef pi::ScopedLock<FastMutex> ScopedLock;

    FastMutex(){}
        /// creates the Mutex.

    ~FastMutex(){}
        /// destroys the Mutex.

    void lock();
        /// Locks the mutex. Blocks if the mutex
        /// is held by another thread.

    void lock(long milliseconds);
        /// Locks the mutex. Blocks up to the given number of milliseconds
        /// if the mutex is held by another thread. Throws a TimeoutException
        /// if the mutex can not be locked within the given timeout.
        ///
        /// Performance Note: On most platforms (including Windows), this member function is
        /// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
        /// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

    bool tryLock();
        /// Tries to lock the mutex. Returns false immediately
        /// if the mutex is already held by another thread.
        /// Returns true if the mutex was successfully locked.

    bool tryLock(long milliseconds);
        /// Locks the mutex. Blocks up to the given number of milliseconds
        /// if the mutex is held by another thread.
        /// Returns true if the mutex was successfully locked.
        ///
        /// Performance Note: On most platforms (including Windows), this member function is
        /// implemented using a loop calling (the equivalent of) tryLock() and Thread::sleep().
        /// On POSIX platforms that support pthread_mutex_timedlock(), this is used.

    void unlock();
        /// Unlocks the mutex so that it can be acquired by
        /// other threads.

private:
    FastMutex(const FastMutex&);
    FastMutex& operator = (const FastMutex&);
};


class PIL_API NullMutex
    /// A NullMutex is an empty mutex implementation
    /// which performs no locking at all. Useful in policy driven design
    /// where the type of mutex used can be now a template parameter allowing the user to switch
    /// between thread-safe and not thread-safe depending on his need
    /// Works with the ScopedLock class
{
public:
    typedef pi::ScopedLock<NullMutex> ScopedLock;

    NullMutex()
        /// Creates the NullMutex.
    {
    }

    ~NullMutex()
        /// Destroys the NullMutex.
    {
    }

    void lock()
        /// Does nothing.
    {
    }

    void lock(long)
        /// Does nothing.
    {
    }

    bool tryLock()
        /// Does nothing and always returns true.
    {
        return true;
    }

    bool tryLock(long)
        /// Does nothing and always returns true.
    {
        return true;
    }

    void unlock()
        /// Does nothing.
    {
    }
};


//
// inlines
//
inline void Mutex::lock()
{
    lockImpl();
}


inline void Mutex::lock(long milliseconds)
{
    if (!tryLockImpl(milliseconds))
        throw TimeoutException();
}


inline bool Mutex::tryLock()
{
    return tryLockImpl();
}


inline bool Mutex::tryLock(long milliseconds)
{
    return tryLockImpl(milliseconds);
}


inline void Mutex::unlock()
{
    unlockImpl();
}


inline void FastMutex::lock()
{
    lockImpl();
}


inline void FastMutex::lock(long milliseconds)
{
    if (!tryLockImpl(milliseconds))
        throw TimeoutException();
}


inline bool FastMutex::tryLock()
{
    return tryLockImpl();
}


inline bool FastMutex::tryLock(long milliseconds)
{
    return tryLockImpl(milliseconds);
}


inline void FastMutex::unlock()
{
    unlockImpl();
}

typedef Mutex::ScopedLock ScopedMutex;

} // namespace pi


#endif // PIL_Mutex_INCLUDED
