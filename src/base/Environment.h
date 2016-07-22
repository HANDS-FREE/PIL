#ifndef PIL_ENVIRONMENT
#define PIL_ENVIRONMENT
//
// Version Information
//
#include "PIL_VERSION.h"

//
// Ensure that PIL_DLL is default unless PIL_STATIC is defined
//
#if defined(_WIN32) && defined(_DLL)
	#if !defined(PIL_DLL) && !defined(PIL_STATIC)
		#define PIL_DLL
	#endif
#endif


//
// The following block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the Foundation_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// Foundation_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//
#if defined(_WIN32) && defined(PIL_DLL)
	#if defined(PIL_EXPORTS)
		#define PIL_API __declspec(dllexport)
	#else
		#define PIL_API __declspec(dllimport)	
	#endif
#endif


#if !defined(PIL_API)
	#define PIL_API
#endif


//
// Automatically link pi_base library for windows.
//
#if defined(_MSC_VER)
	#if !defined(PIL_NO_AUTOMATIC_LIBS) && !defined(PIL_EXPORTS)
        #if defined(PIL_DLL)
			#if defined(_DEBUG)
				#pragma comment(lib, "pi_based.lib")
			#else
				#pragma comment(lib, "pi_base.lib")
			#endif
		#else
			#if defined(_DEBUG)
				#pragma comment(lib, "pi_basemtd.lib")
			#else
				#pragma comment(lib, "pi_basemt.lib")
			#endif
		#endif
	#endif
#endif


//
// Include platform-specific definitions
//
#include "Platform/Platform.h"


//
// The following piece of macro magic joins the two
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard). 
//
#define PIL_JOIN(X, Y) X##Y



#endif // Foundation_Foundation_INCLUDED
