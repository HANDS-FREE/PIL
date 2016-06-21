#ifndef PIL_SingletonHolder_INCLUDED
#define PIL_SingletonHolder_INCLUDED


#include "../Environment.h"
#include "../Thread/Mutex.h"


namespace pi {


template <class S>
class SingletonHolder
    /// This is a helper template class for managing
    /// singleton objects allocated on the heap.
    /// The class ensures proper deletion (including
    /// calling of the destructor) of singleton objects
    /// when the application that created them terminates.
{
public:
    SingletonHolder():
        _pS(0)
        /// Creates the SingletonHolder.
    {
    }

    ~SingletonHolder()
        /// Destroys the SingletonHolder and the singleton
        /// object that it holds.
    {
        delete _pS;
    }

    S* get()
        /// Returns a pointer to the singleton object
        /// hold by the SingletonHolder. The first call
        /// to get will create the singleton.
    {
        FastMutex::ScopedLock lock(_m);
        if (!_pS) _pS = new S;
        return _pS;
    }

private:
    S* _pS;
    FastMutex _m;
};


} // namespace pi


#endif // PIL_SingletonHolder_INCLUDED
