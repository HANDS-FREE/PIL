#ifndef  PIL_Expire_INCLUDED
#define  PIL_Expire_INCLUDED


#include "../Environment.h"
#include "AbstractDelegate.h"
#include "../Time/Timestamp.h"


namespace pi {


template <class TArgs>
class Expire: public AbstractDelegate<TArgs>
    /// Decorator for AbstractDelegate adding automatic
    /// expiring of registrations to AbstractDelegates.
{
public:
    Expire(const AbstractDelegate<TArgs>& p, Timestamp::TimeDiff expireMillisecs):
        AbstractDelegate<TArgs>(p),
        _pDelegate(p.clone()),
        _expire(expireMillisecs*1000)
    {
    }

    Expire(const Expire& expire):
        AbstractDelegate<TArgs>(expire),
        _pDelegate(expire._pDelegate->clone()),
        _expire(expire._expire),
        _creationTime(expire._creationTime)
    {
    }

    ~Expire()
    {
        destroy();
    }

    Expire& operator = (const Expire& expire)
    {
        if (&expire != this)
        {
            delete this->_pDelegate;
            this->_pDelegate    = expire._pDelegate->clone();
            this->_expire       = expire._expire;
            this->_creationTime = expire._creationTime;
            this->_pTarget = expire._pTarget;
        }
        return *this;
    }

    bool notify(const void* sender, TArgs& arguments)
    {
        if (!expired())
            return this->_pDelegate->notify(sender, arguments);
        else
            return false;
    }

    AbstractDelegate<TArgs>* clone() const
    {
        return new Expire(*this);
    }

    void destroy()
    {
        delete this->_pDelegate;
        this->_pDelegate = 0;
    }

    const AbstractDelegate<TArgs>& getDelegate() const
    {
        return *this->_pDelegate;
    }

protected:
    bool expired() const
    {
        return _creationTime.isElapsed(_expire);
    }

    AbstractDelegate<TArgs>* _pDelegate;
    Timestamp::TimeDiff _expire;
    Timestamp _creationTime;

private:
    Expire();
};


} // namespace pi

#endif
