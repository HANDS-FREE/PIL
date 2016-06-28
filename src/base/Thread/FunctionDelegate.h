#ifndef  PIL_FunctionDelegate_INCLUDED
#define  PIL_FunctionDelegate_INCLUDED


#include "../Environment.h"
#include "AbstractDelegate.h"
#include "Mutex.h"


namespace pi {


template <class TArgs, bool hasSender = true, bool senderIsConst = true>
class FunctionDelegate: public AbstractDelegate<TArgs>
    /// Wraps a freestanding function or static member function
    /// for use as a Delegate.
{
public:
    typedef void (*NotifyFunction)(const void*, TArgs&);

    FunctionDelegate(NotifyFunction function):
        _function(function)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _function(delegate._function)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_function = delegate._function;
        }
        return *this;
    }

    bool notify(const void* sender, TArgs& arguments)
    {
        Mutex::ScopedLock lock(_mutex);
        if (_function)
        {
            (*_function)(sender, arguments);
            return true;
        }
        else return false;
    }

    bool equals(const AbstractDelegate<TArgs>& other) const
    {
        const FunctionDelegate* pOtherDelegate = dynamic_cast<const FunctionDelegate*>(other.unwrap());
        return pOtherDelegate && _function == pOtherDelegate->_function;
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new FunctionDelegate(*this);
    }

    void disable()
    {
        Mutex::ScopedLock lock(_mutex);
        _function = 0;
    }

protected:
    NotifyFunction _function;
    Mutex _mutex;

private:
    FunctionDelegate();
};


template <class TArgs>
class FunctionDelegate<TArgs, true, false>: public AbstractDelegate<TArgs>
{
public:
    typedef void (*NotifyFunction)(void*, TArgs&);

    FunctionDelegate(NotifyFunction function):
        _function(function)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _function(delegate._function)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_function = delegate._function;
        }
        return *this;
    }

    bool notify(const void* sender, TArgs& arguments)
    {
        Mutex::ScopedLock lock(_mutex);
        if (_function)
        {
            (*_function)(const_cast<void*>(sender), arguments);
            return true;
        }
        else return false;
    }

    bool equals(const AbstractDelegate<TArgs>& other) const
    {
        const FunctionDelegate* pOtherDelegate = dynamic_cast<const FunctionDelegate*>(other.unwrap());
        return pOtherDelegate && _function == pOtherDelegate->_function;
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new FunctionDelegate(*this);
    }

    void disable()
    {
        Mutex::ScopedLock lock(_mutex);
        _function = 0;
    }

protected:
    NotifyFunction _function;
    Mutex _mutex;

private:
    FunctionDelegate();
};


template <class TArgs, bool senderIsConst>
class FunctionDelegate<TArgs, false, senderIsConst>: public AbstractDelegate<TArgs>
{
public:
    typedef void (*NotifyFunction)(TArgs&);

    FunctionDelegate(NotifyFunction function):
        _function(function)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<TArgs>(delegate),
        _function(delegate._function)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_function = delegate._function;
        }
        return *this;
    }

    bool notify(const void* /*sender*/, TArgs& arguments)
    {
        Mutex::ScopedLock lock(_mutex);
        if (_function)
        {
            (*_function)(arguments);
            return true;
        }
        else return false;
    }

    bool equals(const AbstractDelegate<TArgs>& other) const
    {
        const FunctionDelegate* pOtherDelegate = dynamic_cast<const FunctionDelegate*>(other.unwrap());
        return pOtherDelegate && _function == pOtherDelegate->_function;
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new FunctionDelegate(*this);
    }

    void disable()
    {
        Mutex::ScopedLock lock(_mutex);
        _function = 0;
    }

protected:
    NotifyFunction _function;
    Mutex _mutex;

private:
    FunctionDelegate();
};


template <>
class FunctionDelegate<void, true, true>: public AbstractDelegate<void>
    /// Wraps a freestanding function or static member function
    /// for use as a Delegate.
{
public:
    typedef void (*NotifyFunction)(const void*);

    FunctionDelegate(NotifyFunction function):
        _function(function)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<void>(delegate),
        _function(delegate._function)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_function = delegate._function;
        }
        return *this;
    }

    bool notify(const void* sender)
    {
        Mutex::ScopedLock lock(_mutex);
        if (_function)
        {
            (*_function)(sender);
            return true;
        }
        else return false;
    }

    bool equals(const AbstractDelegate<void>& other) const
    {
        const FunctionDelegate* pOtherDelegate = dynamic_cast<const FunctionDelegate*>(other.unwrap());
        return pOtherDelegate && _function == pOtherDelegate->_function;
    }

    AbstractDelegate<void>* clone() const
    {
        return new FunctionDelegate(*this);
    }

    void disable()
    {
        Mutex::ScopedLock lock(_mutex);
        _function = 0;
    }

protected:
    NotifyFunction _function;
    Mutex _mutex;

private:
    FunctionDelegate();
};


template <>
class FunctionDelegate<void, true, false>: public AbstractDelegate<void>
{
public:
    typedef void (*NotifyFunction)(void*);

    FunctionDelegate(NotifyFunction function):
        _function(function)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<void>(delegate),
        _function(delegate._function)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_function = delegate._function;
        }
        return *this;
    }

    bool notify(const void* sender)
    {
        Mutex::ScopedLock lock(_mutex);
        if (_function)
        {
            (*_function)(const_cast<void*>(sender));
            return true;
        }
        else return false;
    }

    bool equals(const AbstractDelegate<void>& other) const
    {
        const FunctionDelegate* pOtherDelegate = dynamic_cast<const FunctionDelegate*>(other.unwrap());
        return pOtherDelegate && _function == pOtherDelegate->_function;
    }

    AbstractDelegate<void>* clone() const
    {
        return new FunctionDelegate(*this);
    }

    void disable()
    {
        Mutex::ScopedLock lock(_mutex);
        _function = 0;
    }

protected:
    NotifyFunction _function;
    Mutex _mutex;

private:
    FunctionDelegate();
};


template <bool senderIsConst>
class FunctionDelegate<void, false, senderIsConst>: public AbstractDelegate<void>
{
public:
    typedef void (*NotifyFunction)();

    FunctionDelegate(NotifyFunction function):
        _function(function)
    {
    }

    FunctionDelegate(const FunctionDelegate& delegate):
        AbstractDelegate<void>(delegate),
        _function(delegate._function)
    {
    }

    ~FunctionDelegate()
    {
    }

    FunctionDelegate& operator = (const FunctionDelegate& delegate)
    {
        if (&delegate != this)
        {
            this->_function = delegate._function;
        }
        return *this;
    }

    bool notify(const void* /*sender*/)
    {
        Mutex::ScopedLock lock(_mutex);
        if (_function)
        {
            (*_function)();
            return true;
        }
        else return false;
    }

    bool equals(const AbstractDelegate<void>& other) const
    {
        const FunctionDelegate* pOtherDelegate = dynamic_cast<const FunctionDelegate*>(other.unwrap());
        return pOtherDelegate && _function == pOtherDelegate->_function;
    }

    AbstractDelegate<void>* clone() const
    {
        return new FunctionDelegate(*this);
    }

    void disable()
    {
        Mutex::ScopedLock lock(_mutex);
        _function = 0;
    }

protected:
    NotifyFunction _function;
    Mutex _mutex;

private:
    FunctionDelegate();
};



} // namespace pi


#endif // PIL_FunctionDelegate_INCLUDED
