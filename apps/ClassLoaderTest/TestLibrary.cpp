#include "TestPlugin.h"
#include <base/ClassLoader/ClassLibrary.h>
#include <iostream>


extern "C" int PIL_LIBRARY_API gimmeFive();


class PluginA: public TestPlugin
{
public:
    PluginA()
    {
    }

    ~PluginA()
    {
    }

    std::string name() const
    {
        return "PluginA";
    }
};


class PluginB: public TestPlugin
{
public:
    PluginB()
    {
    }

    ~PluginB()
    {
    }

    std::string name() const
    {
        return "PluginB";
    }
};


class PluginC: public TestPlugin
{
public:
    PluginC()
    {
    }

    ~PluginC()
    {
    }

    std::string name() const
    {
        return "PluginC";
    }
};


PIL_BEGIN_MANIFEST(TestPlugin)
    PIL_EXPORT_CLASS(PluginA)
    PIL_EXPORT_CLASS(PluginB)
    PIL_EXPORT_SINGLETON(PluginC)
PIL_END_MANIFEST


void PILInitializeLibrary()
{
    std::cout << "TestLibrary initializing" << std::endl;
}


void PILUninitializeLibrary()
{
    std::cout << "TestLibrary uninitialzing" << std::endl;
}


int gimmeFive()
{
    return 5;
}
