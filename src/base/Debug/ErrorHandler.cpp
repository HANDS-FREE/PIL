#include "ErrorHandler.h"
#include "../Types/SingletonHolder.h"
#include "Assert.h"


namespace pi {


ErrorHandler* ErrorHandler::_pHandler = ErrorHandler::defaultHandler();
FastMutex ErrorHandler::_mutex;


ErrorHandler::ErrorHandler()
{
}


ErrorHandler::~ErrorHandler()
{
}


void ErrorHandler::exception(const Exception& exc)
{
    pi_dbg_error(exc.what());
}


void ErrorHandler::exception(const std::exception& exc)
{
    pi_dbg_error(exc.what());
}


void ErrorHandler::exception()
{
    pi_dbg_error("unknown exception");
}


void ErrorHandler::handle(const Exception& exc)
{
    FastMutex::ScopedLock lock(_mutex);
    try
    {
        _pHandler->exception(exc);
    }
    catch (...)
    {
    }
}


void ErrorHandler::handle(const std::exception& exc)
{
    FastMutex::ScopedLock lock(_mutex);
    try
    {
        _pHandler->exception(exc);
    }
    catch (...)
    {
    }
}


void ErrorHandler::handle()
{
    FastMutex::ScopedLock lock(_mutex);
    try
    {
        _pHandler->exception();
    }
    catch (...)
    {
    }
}


ErrorHandler* ErrorHandler::set(ErrorHandler* pHandler)
{
    pi_dbg_error(pHandler);

    FastMutex::ScopedLock lock(_mutex);
    ErrorHandler* pOld = _pHandler;
    _pHandler = pHandler;
    return pOld;
}


ErrorHandler* ErrorHandler::defaultHandler()
{
    // NOTE: Since this is called to initialize the static _pHandler
    // variable, sh has to be a local static, otherwise we run
    // into static initialization order issues.
    static SingletonHolder<ErrorHandler> sh;
    return sh.get();
}


} // namespace pi
