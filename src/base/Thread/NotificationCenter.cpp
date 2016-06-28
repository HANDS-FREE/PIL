#include "NotificationCenter.h"
#include "Notification.h"
#include "Observer.h"
#include "../Types/AutoPtr.h"
#include "../Types/SingletonHolder.h"


namespace pi {


NotificationCenter::NotificationCenter()
{
}


NotificationCenter::~NotificationCenter()
{
}


void NotificationCenter::addObserver(const AbstractObserver& observer)
{
    Mutex::ScopedLock lock(_mutex);
    _observers.push_back(observer.clone());
}


void NotificationCenter::removeObserver(const AbstractObserver& observer)
{
    Mutex::ScopedLock lock(_mutex);
    for (ObserverList::iterator it = _observers.begin(); it != _observers.end(); ++it)
    {
        if (observer.equals(**it))
        {
            (*it)->disable();
            _observers.erase(it);
            return;
        }
    }
}


bool NotificationCenter::hasObserver(const AbstractObserver& observer) const
{
    Mutex::ScopedLock lock(_mutex);
    for (ObserverList::const_iterator it = _observers.begin(); it != _observers.end(); ++it)
        if (observer.equals(**it)) return true;

    return false;
}


void NotificationCenter::postNotification(Notification::Ptr pNotification)
{
    pi_check_ptr (pNotification);

    ScopedLockWithUnlock<Mutex> lock(_mutex);
    ObserverList observersToNotify(_observers);
    lock.unlock();
    for (ObserverList::iterator it = observersToNotify.begin(); it != observersToNotify.end(); ++it)
    {
        (*it)->notify(pNotification);
    }
}


bool NotificationCenter::hasObservers() const
{
    Mutex::ScopedLock lock(_mutex);

    return !_observers.empty();
}


std::size_t NotificationCenter::countObservers() const
{
    Mutex::ScopedLock lock(_mutex);

    return _observers.size();
}


namespace
{
    static SingletonHolder<NotificationCenter> sh;
}


NotificationCenter& NotificationCenter::defaultCenter()
{
    return *sh.get();
}


} // namespace pi
