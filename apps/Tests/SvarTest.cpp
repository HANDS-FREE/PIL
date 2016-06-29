#include <base/Utils/TestCase.h>
#include <base/Types/VecParament.h>

using namespace pi;
using namespace std;

class SvarTest : public pi::TestCase
{
public:
    SvarTest():pi::TestCase("SvarTest"){}
    virtual void run()
    {
        svar.ParseLine("TestInt=1000");

        pi_assert(svar.GetInt("TestInt",100)==1000);
        pi_assert(svar.GetDouble("TestDouble",1000)==1000);
        pi_assert(svar.GetString("TestString","1000")=="1000");

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

        svar.ParseLine("TestInt=50");
        pi_assert(svar.GetInt("TestInt",1000)==50);

        svar.ParseLine("VecP=[1,2,3,4,5]");
        VecParament<double> vecD=svar.get_var("VecP",VecParament<double>());
        pi_assert(vecD.size()==5);
        pi_assert(vecD.toString()=="[1 2 3 4 5]");

        VecParament<string> vecS=svar.get_var("VecP",VecParament<string>());
        pi_assert(vecS.size()==5);
        pi_assert(vecS.toString()=="[1 2 3 4 5]");
    }
};

SvarTest svarTest;
