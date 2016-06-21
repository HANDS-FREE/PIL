#ifndef PIL_SharedLibrary_INCLUDED
#define PIL_SharedLibrary_INCLUDED


#include "../Environment.h"

#if defined(hpux) || defined(_hpux)
#include "SharedLibrary_HPUX.h"
#elif defined(PIL_OS_FAMILY_UNIX)
#include "SharedLibrary_UNIX.h"
#elif defined(PIL_OS_FAMILY_WINDOWS) && defined(PIL_WIN32_UTF8)
#include "SharedLibrary_WIN32U.h"
#elif defined(PIL_OS_FAMILY_WINDOWS)
#include "SharedLibrary_WIN32.h"
#elif defined(PIL_OS_FAMILY_VMS)
#include "SharedLibrary_VMS.h"
#endif


namespace pi {


class PIL_API SharedLibrary: private SharedLibraryImpl
    /// The SharedLibrary class dynamically
    /// loads shared libraries at run-time.
{
public:
    SharedLibrary();
        /// Creates a SharedLibrary object.

    SharedLibrary(const std::string& path);
        /// Creates a SharedLibrary object and loads a library
        /// from the given path.

    virtual ~SharedLibrary();
        /// Destroys the SharedLibrary. The actual library
        /// remains loaded.

    void load(const std::string& path,int flags=0);
        /// Loads a shared library from the given path.
        /// Throws a LibraryAlreadyLoadedException if
        /// a library has already been loaded.
        /// Throws a LibraryLoadException if the library
        /// cannot be loaded.

    void unload();
        /// Unloads a shared library.

    bool isLoaded() const;
        /// Returns true iff a library has been loaded.

    bool hasSymbol(const std::string& name);
        /// Returns true iff the loaded library contains
        /// a symbol with the given name.

    void* getSymbol(const std::string& name);
        /// Returns the address of the symbol with
        /// the given name. For functions, this
        /// is the entry point of the function.
        /// Throws a NotFoundException if the symbol
        /// does not exist.

    const std::string& getPath() const;
        /// Returns the path of the library, as
        /// specified in a call to load() or the
        /// constructor.

    static std::string suffix();
        /// Returns the platform-specific filename suffix
        /// for shared libraries (including the period).
        /// In debug mode, the suffix also includes a
        /// "d" to specify the debug version of a library.

private:
    SharedLibrary(const SharedLibrary&);
    SharedLibrary& operator = (const SharedLibrary&);
};


} // namespace pi


#endif // PIL_SharedLibrary_INCLUDED
