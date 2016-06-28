#ifndef PIL_Notification_INCLUDED
#define PIL_Notification_INCLUDED

#include <typeinfo>
#include "../Environment.h"
#include "Mutex.h"
#include "../Types/RefCountedObject.h"
#include "../Types/AutoPtr.h"


namespace pi {


class PIL_API Notification: public RefCountedObject
    /// The base class for all notification classes used
    /// with the NotificationCenter and the NotificationQueue
    /// classes.
    /// The Notification class can be used with the AutoPtr
    /// template class.
{
public:
    typedef AutoPtr<Notification> Ptr;

    Notification(){}
        /// Creates the notification.

    virtual std::string name() const
    {
        return typeid(*this).name();
    }
        /// Returns the name of the notification.
        /// The default implementation returns the class name.

protected:
    virtual ~Notification(){}
};


} // namespace pi


#endif // PIL_Notification_INCLUDED
