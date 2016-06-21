#ifndef SCOPEDLOCK_H
#define SCOPEDLOCK_H

namespace pi{

template <class M>
class ScopedLock
    /// A class that simplifies thread synchronization
    /// with a mutex.
    /// The constructor accepts a Mutex and locks it.
    /// The destructor unlocks the mutex.
{
public:
    ScopedLock(M& mutex): _mutex(mutex)
    {
        _mutex.lock();
    }

    ~ScopedLock()
    {
        _mutex.unlock();
    }

private:
    M& _mutex;

    ScopedLock();
    ScopedLock(const ScopedLock&);
    ScopedLock& operator = (const ScopedLock&);
};


template <class M>
class ScopedLockWithUnlock
    /// A class that simplifies thread synchronization
    /// with a mutex.
    /// The constructor accepts a Mutex and locks it.
    /// The destructor unlocks the mutex.
    /// The unlock() member function allows for manual
    /// unlocking of the mutex.
{
public:
    ScopedLockWithUnlock(M& mutex): _pMutex(&mutex)
    {
        _pMutex->lock();
    }

    ~ScopedLockWithUnlock()
    {
        unlock();
    }

    void unlock()
    {
        if (_pMutex)
        {
            _pMutex->unlock();
            _pMutex = 0;
        }
    }

private:
    M* _pMutex;

    ScopedLockWithUnlock();
    ScopedLockWithUnlock(const ScopedLockWithUnlock&);
    ScopedLockWithUnlock& operator = (const ScopedLockWithUnlock&);
};

template <class M>
class ReadLock
{
public:
    ReadLock(M& m_):m(&m_){m->readLock();}
    ~ReadLock(){m->unlock();}
private:
    M* m;
};

template <class M>
class WriteLock
{
public:
    WriteLock(M& m_):m(&m_){m->writeLock();}
    ~WriteLock(){m->unlock();}
private:
    M* m;
};
}// namespace pi
#endif // SCOPEDLOCK_H
