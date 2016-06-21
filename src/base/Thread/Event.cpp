#include "Event.h"

namespace pi {


Event::Event(bool autoReset): EventImpl(autoReset)
{
}


Event::~Event()
{
}


} // namespace Poco
