#ifndef TESTCASE_H
#define TESTCASE_H

#include "../Environment.h"
#include "../Thread/Mutex.h"
#include "../Svar/Scommand.h"
#include <sstream>

namespace pi{

class TestResult
{
public:
    void    add(const std::string& what){
        FastMutex::ScopedLock lock(mutex);
        sst<<what;
    }

    pi::FastMutex     mutex;
    std::stringstream sst;
};

class TestCase
{
public:
    TestCase(const std::string& caseName);

    virtual void run(){}

    void tryRun()
    {
        try
        {
            cerr<<"Trying "<<name<<":\n";
            this->run();
        }
        catch(pi::Exception e)
        {
            result.add(e.displayText);
        }
    }

    void        callAll();
    std::string generateReport();

    std::string               name;
    TestResult                result;
    SvarWithType<TestCase*>&   cases;
};

inline void TestCase::TestCase(const std::string& caseName)
    :name(caseName),cases(SvarWithType<TestCase*>::instance())
{

}

inline void TestCase::callAll()
{

}

inline std::string TestCase::generateReport()
{

}

}

#ifndef PIL_TestMain_H
#define PIL_TestMain_H
int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
    return 0;
}
#endif

#endif // TESTCASE_H
