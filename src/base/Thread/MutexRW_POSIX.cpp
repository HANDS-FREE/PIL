#include "MutexRW_POSIX.h"

namespace pi {

MutexRWImpl::MutexRWImpl()
{
    if (pthread_rwlock_init(&_rwl, NULL))
        throw SystemException("cannot create reader/writer lock");
}


MutexRWImpl::~MutexRWImpl()
{
    pthread_rwlock_destroy(&_rwl);
}


} // namespace Poco
