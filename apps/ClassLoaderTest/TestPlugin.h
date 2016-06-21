#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H
#include <string>

class TestPlugin
{
public:
    TestPlugin(){}
    virtual ~TestPlugin(){}
    virtual std::string name() const = 0;
};
#endif // TESTPLUGIN_H
