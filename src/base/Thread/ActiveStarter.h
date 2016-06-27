#ifndef PIL_ActiveStarter_INCLUDED
#define PIL_ActiveStarter_INCLUDED


#include "../Environment.h"
#include "ThreadPool.h"
#include "ActiveRunnable.h"


namespace pi {


template <class OwnerType>
class ActiveStarter
    /// The default implementation of the StarterType
    /// policy for ActiveMethod. It starts the method
    /// in its own thread, obtained from the default
    /// thread pool.
{
public:
    static void start(OwnerType* /*pOwner*/, ActiveRunnableBase::Ptr pRunnable)
    {
        ThreadPool::defaultPool().start(*pRunnable);
        pRunnable->duplicate(); // The runnable will release itself.
    }
};


} // namespace pi


#endif // PIL_ActiveStarter_INCLUDED
