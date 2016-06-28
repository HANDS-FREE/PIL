#ifndef  PIL_FunctionDelegate_INCLUDED
#define  PIL_FunctionDelegate_INCLUDED


#include "../Environment.h"
#include "AbstractDelegate.h"


namespace pi {


template <class TArgs, bool hasSender = true, bool senderIsConst = true>
class FunctionDelegate: public AbstractDelegate<TArgs>
    /// Wraps a C style function (or a C++ static fucntion) to be used as
    /// a delegate
{
public:
    typedef void (*NotifyMethod)(const void*, TArgs&);

    FunctionDelegate(NotifyMethod method):
        AbstractDelegate<TArgs>(*reinterpret_cast<void**>(&method)),
        _receiverMethod(method)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _receiverMethod(delegate._receiverMethod)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_pTarget        = delegate._pTarget;
            this->_receiverMethod = delegate._receiverMethod;
        }
        return *this;
    }

    bool notify(const void* sender, TArgs& arguments)
    {
        (*_receiverMethod)(sender, arguments);
        return true; // a "standard" delegate never expires
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new FunctionDelegate(*this);
    }

protected:
    NotifyMethod _receiverMethod;

private:
    FunctionDelegate();
};


template <class TArgs>
class FunctionDelegate<TArgs, true, false>: public AbstractDelegate<TArgs>
{
public:
    typedef void (*NotifyMethod)(void*, TArgs&);

    FunctionDelegate(NotifyMethod method):
        AbstractDelegate<TArgs>(*reinterpret_cast<void**>(&method)),
        _receiverMethod(method)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _receiverMethod(delegate._receiverMethod)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_pTarget        = delegate._pTarget;
            this->_receiverMethod = delegate._receiverMethod;
        }
        return *this;
    }

    bool notify(const void* sender, TArgs& arguments)
    {
        (*_receiverMethod)(const_cast<void*>(sender), arguments);
        return true; // a "standard" delegate never expires
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new FunctionDelegate(*this);
    }

protected:
    NotifyMethod _receiverMethod;

private:
    FunctionDelegate();
};


template <class TArgs, bool senderIsConst>
class FunctionDelegate<TArgs, false, senderIsConst>: public AbstractDelegate<TArgs>
{
public:
    typedef void (*NotifyMethod)(TArgs&);

    FunctionDelegate(NotifyMethod method):
        AbstractDelegate<TArgs>(*reinterpret_cast<void**>(&method)),
        _receiverMethod(method)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _receiverMethod(delegate._receiverMethod)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_pTarget        = delegate._pTarget;
            this->_receiverMethod = delegate._receiverMethod;
        }
        return *this;
    }

    bool notify(const void* sender, TArgs& arguments)
    {
        (*_receiverMethod)(arguments);
        return true; // a "standard" delegate never expires
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new FunctionDelegate(*this);
    }

protected:
    NotifyMethod _receiverMethod;

private:
    FunctionDelegate();
};


} // namespace pi


#endif // PIL_FunctionDelegate_INCLUDED
