#ifndef PIL_ClassLibrary_INCLUDED
#define PIL_ClassLibrary_INCLUDED

#include "../Environment.h"
#include "Manifest.h"
#include <typeinfo>


#if defined(_WIN32)
    #define PIL_LIBRARY_API __declspec(dllexport)
#else
    #define PIL_LIBRARY_API
#endif


//
// the entry points for every class library
//
extern "C"
{
    bool PIL_LIBRARY_API PILBuildManifest(pi::ManifestBase* pManifest);
    void PIL_LIBRARY_API PILInitializeLibrary();
    void PIL_LIBRARY_API PILUninitializeLibrary();
}


//
// additional support for named manifests
//
#define PIL_DECLARE_NAMED_MANIFEST(name) \
extern "C"	\
{			\
    bool PIL_LIBRARY_API PIL_JOIN(PILBuildManifest, name)(pi::ManifestBase* pManifest); \
}


//
// Macros to automatically implement PILBuildManifest
//
// usage:
//
// PIL_BEGIN_MANIFEST(MyBaseClass)
//     PIL_EXPORT_CLASS(MyFirstClass)
//     PIL_EXPORT_CLASS(MySecondClass)
//     ...
// PIL_END_MANIFEST
//
#define PIL_BEGIN_MANIFEST_IMPL(fnName, base) \
    bool fnName(pi::ManifestBase* pManifest_)										\
    {																				\
        typedef base _Base;															\
        typedef pi::Manifest<_Base> _Manifest;									\
        std::string requiredType(typeid(_Manifest).name());							\
        std::string actualType(pManifest_->className());							\
        if (requiredType == actualType)												\
        {																			\
            pi::Manifest<_Base>* pManifest = static_cast<_Manifest*>(pManifest_);


#define PIL_BEGIN_MANIFEST(base) \
    PIL_BEGIN_MANIFEST_IMPL(PILBuildManifest, base)


#define PIL_BEGIN_NAMED_MANIFEST(name, base)	\
    PIL_DECLARE_NAMED_MANIFEST(name)			\
    PIL_BEGIN_MANIFEST_IMPL(PIL_JOIN(PILBuildManifest, name), base)


#define PIL_END_MANIFEST \
            return true;	\
        }					\
        else return false;	\
    }


#define PIL_EXPORT_CLASS(cls) \
    pManifest->insert(new pi::MetaObject<cls, _Base>(#cls));


#define PIL_EXPORT_SINGLETON(cls) \
    pManifest->insert(new pi::MetaSingleton<cls, _Base>(#cls));


#endif // PIL_ClassLibrary_INCLUDED
