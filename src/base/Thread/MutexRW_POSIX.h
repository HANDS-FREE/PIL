#ifndef PIL_MutexRW_POSIX_INCLUDED
#define PIL_MutexRW_POSIX_INCLUDED

#include <pthread.h>
#include <errno.h>

#include "../Environment.h"
#include "../Debug/Exception.h"

namespace pi {


class PIL_API MutexRWImpl
{
protected:
    MutexRWImpl();
    ~MutexRWImpl();
    void readLockImpl();
    bool tryReadLockImpl();
    void writeLockImpl();
    bool tryWriteLockImpl();
    void unlockImpl();

private:
    pthread_rwlock_t _rwl;
};


//
// inlines
//
inline void MutexRWImpl::readLockImpl()
{
    if (pthread_rwlock_rdlock(&_rwl))
        throw SystemException("cannot lock reader/writer lock");
}


inline bool MutexRWImpl::tryReadLockImpl()
{
    int rc = pthread_rwlock_tryrdlock(&_rwl);
    if (rc == 0)
        return true;
    else if (rc == EBUSY)
        return false;
    else
        throw SystemException("cannot lock reader/writer lock");

}


inline void MutexRWImpl::writeLockImpl()
{
    if (pthread_rwlock_wrlock(&_rwl))
        throw SystemException("cannot lock reader/writer lock");
}


inline bool MutexRWImpl::tryWriteLockImpl()
{
    int rc = pthread_rwlock_trywrlock(&_rwl);
    if (rc == 0)
        return true;
    else if (rc == EBUSY)
        return false;
    else
        throw SystemException("cannot lock reader/writer lock");

}


inline void MutexRWImpl::unlockImpl()
{
    if (pthread_rwlock_unlock(&_rwl))
        throw SystemException("cannot unlock mutex");
}


} // namespace pi


#endif // PIL_MUTEXRW_POSIX_H
