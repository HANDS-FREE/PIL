#ifndef SYSTEM_H
#define SYSTEM_H
#include <base/types/SPtr.h>

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
