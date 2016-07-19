#ifndef SYSTEM_H
#define SYSTEM_H
#include <base/Types/SPtr.h>

class SystemImpl;

class System
{
public:
    System();
    ~System(){stop();}

    void stop();
private:
    SPtr<SystemImpl> impl;
};

#endif // SYSTEM_H
