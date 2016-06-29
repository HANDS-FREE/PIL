#ifndef PIL_TestMain_H
#define PIL_TestMain_H
#include "TestCase.h"
#include "base/Types/VecParament.h"
#include "base/Time/DateTime.h"
#include <set>

int main(int argc,char** argv)
{
    std::cerr<<"Started test at "<<pi::DateTime()<<std::endl;
    svar.ParseMain(argc,argv);
    pi::TestCase test;
    // filt cases
    if(svar.exist("Cases")||svar.exist("CasesIgnore"))
    {
        VecParament<std::string> CasesVec=svar.get_var("Cases",VecParament<std::string>());
        VecParament<std::string> CasesIgnoreVec=svar.get_var("CasesIgnore",VecParament<std::string>());

        if(CasesVec.size())
        {
            std::set<std::string> CasesMap;
            CasesMap.insert(CasesVec.data.begin(),CasesVec.data.end());
            std::map<std::string,pi::TestCase*> data=test.cases.get_data();
            for(std::map<std::string,pi::TestCase*>::iterator it=data.begin();it!=data.end();it++)
            {
                if(!CasesMap.count(it->first)) test.cases.erase(it->first);
            }
        }

        for(size_t i=0;i<CasesIgnoreVec.size();i++) test.cases.erase(CasesIgnoreVec[i]);
    }
    else
    {
        std::cerr<<"\"Cases\" and \"CasesIgnore\" not setted, test all cases defaultly.\n";
    }

    test.callAll();
    std::cerr<<test.generateReport();
    return 0;
}
#endif // TESTMAIN_H
