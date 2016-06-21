#include "SharedLibrary_UNIX.h"

#if PIL_OS_FAMILY_UNIX
#include "../Debug/Exception.h"
#include <dlfcn.h>


// Note: cygwin is missing RTLD_LOCAL, set it to 0
#if defined(__CYGWIN__) && !defined(RTLD_LOCAL)
#define RTLD_LOCAL 0
#endif
#include <iostream>


namespace pi {


FastMutex SharedLibraryImpl::_mutex;


SharedLibraryImpl::SharedLibraryImpl()
{
    _handle = 0;
}


SharedLibraryImpl::~SharedLibraryImpl()
{
}

void SharedLibraryImpl::loadImpl(const std::string& path, int flags)
{
    FastMutex::ScopedLock lock(_mutex);

    if (_handle) throw LibraryAlreadyLoadedException(path);
    int realFlags = RTLD_LAZY;
    if (flags & SHLIB_LOCAL_IMPL)
        realFlags |= RTLD_LOCAL;
    else
        realFlags |= RTLD_GLOBAL;
    _handle = dlopen(path.c_str(), realFlags);
    if (!_handle)
    {
        const char* err = dlerror();
        std::cerr<<"Can't open file "<<path<<" since "<<err<<std::endl;
        throw LibraryLoadException(err ? std::string(err) : path);
    }
    _path = path;
}


void SharedLibraryImpl::unloadImpl()
{
    FastMutex::ScopedLock lock(_mutex);

    if (_handle)
    {
        dlclose(_handle);
        _handle = 0;
    }
}


bool SharedLibraryImpl::isLoadedImpl() const
{
    return _handle != 0;
}


void* SharedLibraryImpl::findSymbolImpl(const std::string& name)
{
    FastMutex::ScopedLock lock(_mutex);

    void* result = 0;
    if (_handle)
    {
        result = dlsym(_handle, name.c_str());
    }
    return result;
}


const std::string& SharedLibraryImpl::getPathImpl() const
{
    return _path;
}


std::string SharedLibraryImpl::suffixImpl()
{
#if defined(__APPLE__)
    #if defined(_DEBUG)
        return "d.dylib";
    #else
        return ".dylib";
    #endif
#elif defined(hpux) || defined(_hpux)
    #if defined(_DEBUG)
        return "d.sl";
    #else
        return ".sl";
    #endif
#elif defined(__CYGWIN__)
    #if defined(_DEBUG)
        return "d.dll";
    #else
        return ".dll";
    #endif
#else
    #if defined(_DEBUG)
        return "d.so";
    #else
        return ".so";
    #endif
#endif
}


} // namespace pi
#endif //PIL_OS_FAMILY_UNIX
