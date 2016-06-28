#ifndef PIL_AbstractObserver_INCLUDED
#define PIL_AbstractObserver_INCLUDED


#include "../Environment.h"
#include "Notification.h"


namespace pi {


class PIL_API AbstractObserver
    /// The base class for all instantiations of
    /// the Observer and NObserver template classes.
{
public:
    AbstractObserver(){}
    AbstractObserver(const AbstractObserver& observer){}
    virtual ~AbstractObserver(){}

    AbstractObserver& operator = (const AbstractObserver& observer){return *this;}

    virtual void notify(Notification* pNf) const = 0;
    virtual bool equals(const AbstractObserver& observer) const = 0;
    virtual bool accepts(Notification* pNf) const = 0;
    virtual AbstractObserver* clone() const = 0;
    virtual void disable() = 0;
};


} // namespace pi


#endif // PIL_AbstractObserver_INCLUDED
