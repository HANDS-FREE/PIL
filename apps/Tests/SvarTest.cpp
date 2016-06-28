#include <base/Utils/TestCase.h>

using namespace pi;
using namespace std;

class SvarTest : public pi::TestCase
{
public:
    SvarTest():pi::TestCase("SvarTest"){}
    virtual void run()
    {
        pi_assert(svar.GetInt("TestInt",1000)==1000);
        pi_assert(svar.GetDouble("TestInt",1000)==1000);
        pi_assert(svar.GetString("TestInt","1000")=="1000");

        int &test_int=svar.GetInt("TestInt",1000);
        double &test_double=svar.GetDouble("TestDouble",1000);
        string &test_string=svar.GetString("TestString","1000");

        test_int=10000;
        test_double=10000;
        test_string="10000";

        pi_assert(svar.GetInt("TestInt",1000)==10000);
        pi_assert(svar.GetDouble("TestDouble",1000)==10000);
        pi_assert(svar.GetString("TestString","1000")=="10000");

        svar.i["TestInt"]=0;
        pi_assert(svar.GetInt("TestInt",1000)==0);


    }
};

SvarTest svarTest;
