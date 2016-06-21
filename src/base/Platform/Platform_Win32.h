#ifndef Foundation_Platform_WIN32_INCLUDED
#define Foundation_Platform_WIN32_INCLUDED


// Verify that we're built with the multithreaded 
// versions of the runtime libraries
#if defined(_MSC_VER) && !defined(_MT)
	#error Must compile with /MD, /MDd, /MT or /MTd
#endif


// Check debug/release settings consistency
#if defined(NDEBUG) && defined(_DEBUG)
	#error Inconsistent build settings (check for /MD[d])
#endif


#if defined(_WIN32)
	#if !defined(_WIN32_WINNT)
		#define _WIN32_WINNT 0x0500
	#endif
	#if !defined(WIN32_LEAN_AND_MEAN) && !defined(PIL_BLOATED_WIN32)
		#define WIN32_LEAN_AND_MEAN
	#endif
#endif


// Unicode Support
#if defined(UNICODE) && !defined(PIL_WIN32_UTF8)
	#define PIL_WIN32_UTF8
#endif


// Turn off some annoying warnings
#if defined(_MSC_VER)
	#pragma warning(disable:4018) // signed/unsigned comparison
	#pragma warning(disable:4251) // ... needs to have dll-interface warning 
	#pragma warning(disable:4355) // 'this' : used in base member initializer list
	#pragma warning(disable:4996) // VC++ 8.0 deprecation warnings
	#pragma warning(disable:4351) // new behavior: elements of array '...' will be default initialized
	#pragma warning(disable:4675) // resolved overload was found by argument-dependent lookup
#endif


#if defined(__INTEL_COMPILER)
	#pragma warning(disable:1738) // base class dllexport/dllimport specification differs from that of the derived class
	#pragma warning(disable:1478) // function ... was declared "deprecated"
	#pragma warning(disable:1744) // field of class type without a DLL interface used in a class with a DLL interface
#endif


#endif // Platform_WIN32_INCLUDED

