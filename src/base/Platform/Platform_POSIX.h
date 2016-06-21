#ifndef PIL_Platform_POSIX_INCLUDED
#define PIL_Platform_POSIX_INCLUDED


//
// PA-RISC based HP-UX platforms have some issues...
//
#if defined(hpux) || defined(_hpux)
	#if defined(__hppa) || defined(__hppa__)
		#define POCO_NO_SYS_SELECT_H 1
		#if defined(__HP_aCC)
			#define POCO_NO_TEMPLATE_ICOMPARE 1
		#endif
	#endif
#endif


#endif // PIL_Platform_POSIX_INCLUDED

