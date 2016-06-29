#ifndef TESTCASE_H
#define TESTCASE_H

#include "../Debug/Exception.h"
#include "../Environment.h"
#include "../Thread/Mutex.h"
#include "../Svar/Scommand.h"
#include "../Path/Path.h"
#include <sstream>

namespace pi{

class TestException : public Exception
{
public:
    TestException(const std::string& msg,const std::string& File,const std::string& Function,const int Line)
        :Exception(msg),file(File),function(Function),line(Line){}

    std::string file,function;
    int         line;
};

#define TESTEXCEPTION(msg) TestException(msg,__FILE__,__FUNCTION__,__LINE__)

#ifdef pi_assert
    #undef pi_assert
    #undef pi_assert2
#endif

#define pi_assert(f) \
    if(!(f)) \
        {\
            throw TESTEXCEPTION("");\
        }

#define pi_assert2(f,s) \
    if(!(f)) \
        {\
            throw TESTEXCEPTION(s);\
        }

#define fail(s) pi_assert2(false,s)

class TestResult
{
public:
    void    add(const std::string& what){
        FastMutex::ScopedLock lock(mutex);
        sst<<what;
    }

    static std::string rightPad(const std::string &str, const size_t total_len, bool truncate_if_larger)
    {
        std::string r = str;
        if (r.size()<total_len || truncate_if_larger)
            r.resize(total_len,' ');
        return r;
    }

    static std::string  aux_format_string_multilines(const std::string &s, const size_t len,const size_t leftPad=0)
    {
        std::string ret;

        for (size_t p=0;p<s.size();p+=len)
        {
            ret+=rightPad(s.c_str()+p,len,true);
            if (p+len<s.size())
            {
                ret+="\n";
                ret+=rightPad("",leftPad,true);
            }
        }
        return ret;
    }

    friend std::ostream& operator <<(std::ostream& os,const TestResult& r)
    {
        if(!r.sst.str().size())
        {
            os<<"PASSED";
        }
        else
            os<<"FAILED at "<<TestResult::aux_format_string_multilines(r.sst.str(),52,35);
        return os;
    }

    pi::FastMutex     mutex;
    std::stringstream sst;
};


class TestCase
{
public:
    TestCase(const std::string& caseName="")
        :name(caseName),cases(SvarWithType<TestCase*>::instance())
    {
        if(name.size())
            cases[name]=this;
    }

    virtual void run(){}

    void tryRun()
    {
        std::cerr<<"-------------------------------------------------\n";
        std::cerr<<"Calling "<<this->name<<"...  ";
        std::stringstream detail;
        try
        {
            this->run();
        }
        catch(pi::TestException& e)
        {
            std::stringstream sst;
            sst<<"Line:"<<e.line<<",Func:"<<e.function;
            if(svar.GetInt("Test.EchoFile",1))
                sst<<",File:"<<Path(e.file).getFileName();
            result.add(sst.str());
            detail<<e.displayText();
        }
        catch(pi::Exception& e)
        {
            result.add(e.displayText());
        }
        catch(std::exception& e)
        {
            result.add(e.what());
        }
        std::cerr<<result<<detail.str()<<"\n";
    }

    void        callAll();
    std::string generateReport();

    std::string               name;
    TestResult                result;
    SvarWithType<TestCase*>&  cases;
};

inline void TestCase::callAll()
{
    std::map<std::string,TestCase*> data=cases.get_data();
    for(std::map<std::string,TestCase*>::iterator it=data.begin();it!=data.end();it++)
    {
        TestCase* testCase=it->second;
        testCase->tryRun();
    }
}

inline std::string TestCase::generateReport()
{
    std::ostringstream ost;
    ost<<"------------------------------------  Test report  ------------------------------------\n";
    ost<<"         TestCase        Result                       DebugInfo                        \n";
    ost<<"---------------------------------------------------------------------------------------\n";
    std::map<std::string,TestCase*> data=cases.get_data();
    for(std::map<std::string,TestCase*>::iterator it=data.begin();it!=data.end();it++)
    {
        TestCase* testCase=it->second;
        ost<<TestResult::aux_format_string_multilines(testCase->name,25);
        ost<<testCase->result<<std::endl;
    }

    ost<<"--------------------------------- End of Test report ----------------------------------\n";

    return ost.str();
}

}



#endif // TESTCASE_H
