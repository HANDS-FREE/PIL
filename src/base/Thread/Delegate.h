#ifndef  Foundation_Delegate_INCLUDED
#define  Foundation_Delegate_INCLUDED


#include "../Environment.h"
#include "AbstractDelegate.h"
#include "FunctionDelegate.h"
#include "Expire.h"


namespace pi {


template <class TObj, class TArgs, bool withSender=true>
class Delegate: public AbstractDelegate<TArgs>
{
public:
    typedef void (TObj::*NotifyMethod)(const void*, TArgs&);

    Delegate(TObj* obj, NotifyMethod method):
        AbstractDelegate<TArgs>(obj),
        _receiverObject(obj),
        _receiverMethod(method)
    {
    }

    Delegate(const Delegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _receiverObject(delegate._receiverObject),
        _receiverMethod(delegate._receiverMethod)
    {
    }

    ~Delegate()
    {
    }

    Delegate& operator = (const Delegate& delegate)
    {
        if (&delegate != this)
        {
            this->_pTarget        = delegate._pTarget;
            this->_receiverObject = delegate._receiverObject;
            this->_receiverMethod = delegate._receiverMethod;
        }
        return *this;
    }

    bool notify(const void* sender, TArgs& arguments)
    {
        (_receiverObject->*_receiverMethod)(sender, arguments);
        return true; // a "standard" delegate never expires
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new Delegate(*this);
    }

protected:
    TObj*        _receiverObject;
    NotifyMethod _receiverMethod;

private:
    Delegate();
};


template <class TObj, class TArgs>
class Delegate<TObj, TArgs, false>: public AbstractDelegate<TArgs>
{
public:
    typedef void (TObj::*NotifyMethod)(TArgs&);

    Delegate(TObj* obj, NotifyMethod method):
        AbstractDelegate<TArgs>(obj),
        _receiverObject(obj),
        _receiverMethod(method)
    {
    }

    Delegate(const Delegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _receiverObject(delegate._receiverObject),
        _receiverMethod(delegate._receiverMethod)
    {
    }

    ~Delegate()
    {
    }

    Delegate& operator = (const Delegate& delegate)
    {
        if (&delegate != this)
        {
            this->_pTarget        = delegate._pTarget;
            this->_receiverObject = delegate._receiverObject;
            this->_receiverMethod = delegate._receiverMethod;
        }
        return *this;
    }

    virtual bool notify(const void*, TArgs& arguments)
    {
        (_receiverObject->*_receiverMethod)(arguments);
        return true; // a "standard" delegate never expires
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new Delegate(*this);
    }

protected:
    TObj*        _receiverObject;
    NotifyMethod _receiverMethod;

private:
    Delegate();
};


template <class TObj, class TArgs>
static Delegate<TObj, TArgs, true> delegate(TObj* pObj, void (TObj::*NotifyMethod)(const void*, TArgs&))
{
    return Delegate<TObj, TArgs, true>(pObj, NotifyMethod);
}


template <class TObj, class TArgs>
static Delegate<TObj, TArgs, false> delegate(TObj* pObj, void (TObj::*NotifyMethod)(TArgs&))
{
    return Delegate<TObj, TArgs, false>(pObj, NotifyMethod);
}



template <class TObj, class TArgs>
static Expire<TArgs> delegate(TObj* pObj, void (TObj::*NotifyMethod)(const void*, TArgs&), Timestamp::TimeDiff expireMillisecs)
{
    return Expire<TArgs>(Delegate<TObj, TArgs, true>(pObj, NotifyMethod), expireMillisecs);
}


template <class TObj, class TArgs>
static Expire<TArgs> delegate(TObj* pObj, void (TObj::*NotifyMethod)(TArgs&), Timestamp::TimeDiff expireMillisecs)
{
    return Expire<TArgs>(Delegate<TObj, TArgs, false>(pObj, NotifyMethod), expireMillisecs);
}


template <class TArgs>
static Expire<TArgs> delegate(void (*NotifyMethod)(const void*, TArgs&), Timestamp::TimeDiff expireMillisecs)
{
    return Expire<TArgs>(FunctionDelegate<TArgs, true, true>(NotifyMethod), expireMillisecs);
}


template <class TArgs>
static Expire<TArgs> delegate(void (*NotifyMethod)(void*, TArgs&), Timestamp::TimeDiff expireMillisecs)
{
    return Expire<TArgs>(FunctionDelegate<TArgs, true, false>(NotifyMethod), expireMillisecs);
}


template <class TArgs>
static Expire<TArgs> delegate(void (*NotifyMethod)(TArgs&), Timestamp::TimeDiff expireMillisecs)
{
    return Expire<TArgs>(FunctionDelegate<TArgs, false>( NotifyMethod), expireMillisecs);
}


template <class TArgs>
static FunctionDelegate<TArgs, true, true> delegate(void (*NotifyMethod)(const void*, TArgs&))
{
    return FunctionDelegate<TArgs, true, true>(NotifyMethod);
}


template <class TArgs>
static FunctionDelegate<TArgs, true, false> delegate(void (*NotifyMethod)(void*, TArgs&))
{
    return FunctionDelegate<TArgs, true, false>(NotifyMethod);
}


template <class TArgs>
static FunctionDelegate<TArgs, false> delegate(void (*NotifyMethod)(TArgs&))
{
    return FunctionDelegate<TArgs, false>(NotifyMethod);
}


} // namespace pi


#endif
