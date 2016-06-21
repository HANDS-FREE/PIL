#ifndef PIL_ThreadLocal_INCLUDED
#define PIL_ThreadLocal_INCLUDED

#include <map>

#include "Thread.h"

namespace pi {

#if PIL_OS_FAMILY_UNIX

template<class C> class ThreadLocal
{
private:
    pthread_key_t key;

    static void deleter(void* v)
    {
        delete static_cast<C*>(v);
    }

public:
    ThreadLocal()
    {
        pthread_key_create(&key, deleter);
        pthread_setspecific(key, new C);
    }

    ~ThreadLocal()
    {
        deleter(pthread_getspecific(key));
        pthread_setspecific(key, 0);
        pthread_key_delete(key);
    }


    C& operator()()
    {
        return *static_cast<C*>(pthread_getspecific(key));
    }
};

#else
#endif

} // namespace pi


#endif // PIL_ThreadLocal_INCLUDED
