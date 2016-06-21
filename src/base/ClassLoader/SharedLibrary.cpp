#include "SharedLibrary.h"
#include "../Debug/Exception.h"

namespace pi {

SharedLibrary::SharedLibrary()
{
}


SharedLibrary::SharedLibrary(const std::string& path)
{
    loadImpl(path, 0);
}

SharedLibrary::~SharedLibrary()
{
}

void SharedLibrary::load(const std::string& path, int flags)
{
    loadImpl(path, flags);
}


void SharedLibrary::unload()
{
    unloadImpl();
}


bool SharedLibrary::isLoaded() const
{
    return isLoadedImpl();
}


bool SharedLibrary::hasSymbol(const std::string& name)
{
    return findSymbolImpl(name) != 0;
}


void* SharedLibrary::getSymbol(const std::string& name)
{
    void* result = findSymbolImpl(name);
    if (result)
        return result;
    else
        throw NotFoundException(name);
}


const std::string& SharedLibrary::getPath() const
{
    return getPathImpl();
}


std::string SharedLibrary::suffix()
{
    return suffixImpl();
}


} // namespace pi
