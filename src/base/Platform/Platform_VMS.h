#ifndef PIL_Platform_VMS_INCLUDED
#define PIL_Platform_VMS_INCLUDED


// Define the PIL_DESCRIPTOR_STRING and PIL_DESCRIPTOR_LITERAL
// macros which we use instead of $DESCRIPTOR and $DESCRIPTOR64. 
// Our macros work with both 32bit and 64bit pointer sizes.
#if __INITIAL_POINTER_SIZE != 64
    #define PIL_DESCRIPTOR_STRING(name, string) \
		struct dsc$descriptor_s name =	\
		{								\
			string.size(),				\
			DSC$K_DTYPE_T,				\
			DSC$K_CLASS_S,				\
			(char*) string.data()		\
		}
    #define PIL_DESCRIPTOR_LITERAL(name, string) \
		struct dsc$descriptor_s name =	\
		{								\
			sizeof(string) - 1,			\
			DSC$K_DTYPE_T,				\
			DSC$K_CLASS_S,				\
			(char*) string				\
		}
#else
    #define PIL_DESCRIPTOR_STRING(name, string) \
		struct dsc64$descriptor_s name =\
		{								\
			1,							\
			DSC64$K_DTYPE_T,			\
			DSC64$K_CLASS_S,			\
			-1,							\
			string.size(),				\
			(char*) string.data()		\
		}
    #define PIL_DESCRIPTOR_LITERAL(name, string) \
		struct dsc64$descriptor_s name =\
		{								\
			1,							\
			DSC64$K_DTYPE_T,			\
			DSC64$K_CLASS_S,			\
			-1,							\
			sizeof(string) - 1,			\
			(char*) string				\
		}
#endif


// No <sys/select.h> header file
#define PIL_NO_SYS_SELECT_H


#endif // PIL_Platform_VMS_INCLUDED
