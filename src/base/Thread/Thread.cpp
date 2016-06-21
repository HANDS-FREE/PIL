//
// Thread.cpp
//
// $Id: //PIL/1.4/PIL/src/Thread.cpp#2 $
//
// Library: PIL
// Package: Threading
// Module:  Thread
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Thread.h"
#include "Mutex.h"
#include "../Debug/Exception.h"
#include "AtomicCounter.h"
#include <sstream>

namespace pi {


namespace {

class RunnableHolder: public Runnable
{
public:
    RunnableHolder(Runnable& target):
        _target(target)
    {
    }

    ~RunnableHolder()
    {
    }

    void run()
    {
        _target.run();
    }

private:
    Runnable& _target;
};


class CallableHolder: public Runnable
{
public:
    CallableHolder(Thread::Callable callable, void* pData):
        _callable(callable),
        _pData(pData)
    {
    }

    ~CallableHolder()
    {
    }

    void run()
    {
        _callable(_pData);
    }

private:
    Thread::Callable _callable;
    void* _pData;
};


} // namespace


Thread::Thread():
    _id(uniqueId()),
    _name(makeName()),
    _needStop(false)
{
}


Thread::Thread(const std::string& name):
    _id(uniqueId()),
    _name(name)
{
}


Thread::~Thread()
{
}


void Thread::setPriority(Priority prio)
{
    setPriorityImpl(prio);
}


Thread::Priority Thread::getPriority() const
{
    return Priority(getPriorityImpl());
}


void Thread::start(Runnable* target)
{
    if(!target)
        startImpl(new RunnableHolder(*this));
    else
        startImpl(new RunnableHolder(*target));
}


void Thread::start(Callable target, void* pData)
{
    startImpl(new CallableHolder(target, pData));
}


void Thread::join()
{
    joinImpl();
}


void Thread::join(long milliseconds)
{
    if (!joinImpl(milliseconds))
        throw TimeoutException();
}


bool Thread::tryJoin(long milliseconds)
{
    return joinImpl(milliseconds);
}


bool Thread::trySleep(long milliseconds)
{
    throw Exception("Thread::trySleep not implemented!");
    return false;
//    Thread* pT = Thread::current();
////    PIL_check_ptr(pT);
//    return !(pT->_event.tryWait(milliseconds));
}


void Thread::wakeUp()
{
//    _event.set();
}

std::string Thread::makeName()
{
    std::ostringstream name;
    name << '#' << _id;
    return name.str();
}


int Thread::uniqueId()
{
    static pi::AtomicCounter counter;
    return ++counter;
}


void Thread::setName(const std::string& name)
{
    FastMutex::ScopedLock lock(_mutex);

    _name = name;
}


} // namespace PIL
