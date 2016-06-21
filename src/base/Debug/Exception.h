#ifndef PIL_Exception_INCLUDED
#define PIL_Exception_INCLUDED


#include "../Environment.h"
#include <stdexcept>


namespace pi {


class PIL_API Exception: public std::exception
    /// This is the base class for all exceptions defined
    /// in the PIL class library.
{
public:
    Exception(const std::string& msg, int code = 0);
        /// Creates an exception.

    Exception(const std::string& msg, const std::string& arg, int code = 0);
        /// Creates an exception.

    Exception(const std::string& msg, const Exception& nested, int code = 0);
        /// Creates an exception and stores a clone
        /// of the nested exception.

    Exception(const Exception& exc);
        /// Copy constructor.

    ~Exception() throw();
        /// Destroys the exception and deletes the nested exception.

    Exception& operator = (const Exception& exc);
        /// Assignment operator.

    virtual const char* name() const throw();
        /// Returns a static string describing the exception.

    virtual const char* className() const throw();
        /// Returns the name of the exception class.

    virtual const char* what() const throw();
        /// Returns a static string describing the exception.
        ///
        /// Same as name(), but for compatibility with std::exception.

    const Exception* nested() const;
        /// Returns a pointer to the nested exception, or
        /// null if no nested exception exists.

    const std::string& message() const;
        /// Returns the message text.

    int code() const;
        /// Returns the exception code if defined.

    std::string displayText() const;
        /// Returns a string consisting of the
        /// message name and the message text.

    virtual Exception* clone() const;
        /// Creates an exact copy of the exception.
        ///
        /// The copy can later be thrown again by
        /// invoking rethrow() on it.

    virtual void rethrow() const;
        /// (Re)Throws the exception.
        ///
        /// This is useful for temporarily storing a
        /// copy of an exception (see clone()), then
        /// throwing it again.

protected:
    Exception(int code = 0);
        /// Standard constructor.

    void message(const std::string& msg);
        /// Sets the message for the exception.

    void extendedMessage(const std::string& arg);
        /// Sets the extended message for the exception.

private:
    std::string _msg;
    Exception*  _pNested;
    int			_code;
};


//
// inlines
//
inline const Exception* Exception::nested() const
{
    return _pNested;
}


inline const std::string& Exception::message() const
{
    return _msg;
}


inline void Exception::message(const std::string& msg)
{
    _msg = msg;
}


inline int Exception::code() const
{
    return _code;
}


//
// Macros for quickly declaring and implementing exception classes.
// Unfortunately, we cannot use a template here because character
// pointers (which we need for specifying the exception name)
// are not allowed as template arguments.
//
#define PIL_DECLARE_EXCEPTION(API, CLS, BASE) \
    class API CLS: public BASE														\
    {																				\
    public:																			\
        CLS(int code = 0);															\
        CLS(const std::string& msg, int code = 0);									\
        CLS(const std::string& msg, const std::string& arg, int code = 0);			\
        CLS(const std::string& msg, const pi::Exception& exc, int code = 0);		\
        CLS(const CLS& exc);														\
        ~CLS() throw();																\
        CLS& operator = (const CLS& exc);											\
        const char* name() const throw();											\
        const char* className() const throw();										\
        pi::Exception* clone() const;												\
        void rethrow() const;														\
    };


#define PIL_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)													\
    CLS::CLS(int code): BASE(code)																	\
    {																								\
    }																								\
    CLS::CLS(const std::string& msg, int code): BASE(msg, code)										\
    {																								\
    }																								\
    CLS::CLS(const std::string& msg, const std::string& arg, int code): BASE(msg, arg, code)		\
    {																								\
    }																								\
    CLS::CLS(const std::string& msg, const pi::Exception& exc, int code): BASE(msg, exc, code)	\
    {																								\
    }																								\
    CLS::CLS(const CLS& exc): BASE(exc)																\
    {																								\
    }																								\
    CLS::~CLS() throw()																				\
    {																								\
    }																								\
    CLS& CLS::operator = (const CLS& exc)															\
    {																								\
        BASE::operator = (exc);																		\
        return *this;																				\
    }																								\
    const char* CLS::name() const throw()															\
    {																								\
        return NAME;																				\
    }																								\
    const char* CLS::className() const throw()														\
    {																								\
        return typeid(*this).name();																\
    }																								\
    pi::Exception* CLS::clone() const																\
    {																								\
        return new CLS(*this);																		\
    }																								\
    void CLS::rethrow() const																		\
    {																								\
        throw *this;																				\
    }


//
// Standard exception classes
//
PIL_DECLARE_EXCEPTION(PIL_API, LogicException, Exception)
PIL_DECLARE_EXCEPTION(PIL_API, AssertionViolationException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, NullPointerException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, NullValueException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, BugcheckException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, InvalidArgumentException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, NotImplementedException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, RangeException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, IllegalStateException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, InvalidAccessException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, SignalException, LogicException)
PIL_DECLARE_EXCEPTION(PIL_API, UnhandledException, LogicException)

PIL_DECLARE_EXCEPTION(PIL_API, RuntimeException, Exception)
PIL_DECLARE_EXCEPTION(PIL_API, NotFoundException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, ExistsException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, TimeoutException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, SystemException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, RegularExpressionException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, LibraryLoadException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, LibraryAlreadyLoadedException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, NoThreadAvailableException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, PropertyNotSupportedException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, PoolOverflowException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, NoPermissionException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, OutOfMemoryException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, DataException, RuntimeException)

PIL_DECLARE_EXCEPTION(PIL_API, DataFormatException, DataException)
PIL_DECLARE_EXCEPTION(PIL_API, SyntaxException, DataException)
PIL_DECLARE_EXCEPTION(PIL_API, CircularReferenceException, DataException)
PIL_DECLARE_EXCEPTION(PIL_API, PathSyntaxException, SyntaxException)
PIL_DECLARE_EXCEPTION(PIL_API, IOException, RuntimeException)
PIL_DECLARE_EXCEPTION(PIL_API, ProtocolException, IOException)
PIL_DECLARE_EXCEPTION(PIL_API, FileException, IOException)
PIL_DECLARE_EXCEPTION(PIL_API, FileExistsException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, FileNotFoundException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, PathNotFoundException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, FileReadOnlyException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, FileAccessDeniedException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, CreateFileException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, OpenFileException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, WriteFileException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, ReadFileException, FileException)
PIL_DECLARE_EXCEPTION(PIL_API, UnknownURISchemeException, RuntimeException)

PIL_DECLARE_EXCEPTION(PIL_API, ApplicationException, Exception)
PIL_DECLARE_EXCEPTION(PIL_API, BadCastException, RuntimeException)


} // namespace PIL


#endif // PIL_Exception_INCLUDED
