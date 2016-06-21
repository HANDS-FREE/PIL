#include "../Environment.h"

#if     PIL_OS_FAMILY_UNIX
#ifndef PIL_SharedLibrary_UNIX_INCLUDED
#define PIL_SharedLibrary_UNIX_INCLUDED

#include "../Thread/Mutex.h"

namespace pi {

class PIL_API SharedLibraryImpl
{
protected:
    enum Flags
    {
        SHLIB_GLOBAL_IMPL = 1,
        SHLIB_LOCAL_IMPL  = 2
    };

    SharedLibraryImpl();
    ~SharedLibraryImpl();
    void loadImpl(const std::string& path, int flags);
    void unloadImpl();
    bool isLoadedImpl() const;
    void* findSymbolImpl(const std::string& name);
    const std::string& getPathImpl() const;
    static std::string suffixImpl();

private:
    std::string _path;
    void* _handle;
    static FastMutex _mutex;
};


} // namespace pi


#endif // PIL_SharedLibrary_UNIX_INCLUDED
#endif // PIL_OS_FAMILY_UNIX
