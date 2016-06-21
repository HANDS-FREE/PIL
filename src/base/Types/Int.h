#ifndef PIL_Int_INCLUDED
#define PIL_Int_INCLUDED


#include "base/Environment.h"

namespace pi {

#if defined(_MSC_VER)
	//
	// Windows/Visual C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed __int64         Int64;
	typedef unsigned __int64       UInt64;
	#if defined(_WIN64)
		#define PIL_PTR_IS_64_BIT 1
		typedef signed __int64     IntPtr;
		typedef unsigned __int64   UIntPtr;
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
	#endif
	#define PIL_HAVE_INT64 1
#elif defined(__GNUC__)
	//
	// Unix/GCC
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__LP64__)
		#define PIL_PTR_IS_64_BIT 1
		#define PIL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define PIL_HAVE_INT64 1
#elif defined(__DECCXX)
	//
	// Compaq C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed __int64         Int64;
	typedef unsigned __int64       UInt64;
	#if defined(__VMS)
		#if defined(__32BITS)
			typedef signed long    IntPtr;
			typedef unsigned long  UIntPtr;
		#else
			typedef Int64          IntPtr;
			typedef UInt64         UIntPtr;
			#define PIL_PTR_IS_64_BIT 1
		#endif
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
		#define PIL_PTR_IS_64_BIT 1
		#define PIL_LONG_IS_64_BIT 1
	#endif
	#define PIL_HAVE_INT64 1
#elif defined(__HP_aCC)
	//
	// HP Ansi C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__LP64__)
		#define PIL_PTR_IS_64_BIT 1
		#define PIL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define PIL_HAVE_INT64 1
#elif defined(__SUNPRO_CC)
	//
	// SUN Forte C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__sparcv9)
		#define PIL_PTR_IS_64_BIT 1
		#define PIL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define PIL_HAVE_INT64 1
#elif defined(__IBMCPP__) 
	//
	// IBM XL C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__64BIT__)
		#define PIL_PTR_IS_64_BIT 1
		#define PIL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define PIL_HAVE_INT64 1
#elif defined(__sgi) 
	//
	// MIPSpro C++
	//
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if _MIPS_SZLONG == 64
		#define PIL_PTR_IS_64_BIT 1
		#define PIL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define PIL_HAVE_INT64 1
#endif
typedef UInt8           uint8_t;
typedef UInt16          uint16_t;
typedef UInt32          uint32_t;
typedef UInt64          uint64_t;
typedef Int8            int8_t;
typedef Int16           int16_t;
typedef Int32           int32_t;
typedef Int64           int64_t;

typedef uint8_t         ru8;
typedef uint16_t        ru16;
typedef uint32_t        ru32;
typedef uint64_t        ru64;

typedef int8_t          ri8;
typedef int16_t         ri16;
typedef int32_t         ri32;
typedef int64_t         ri64;

typedef float           rf32;
typedef double          rf64;

typedef unsigned char   byte;


} // namespace PIL


#endif // PIL_Int_INCLUDED

