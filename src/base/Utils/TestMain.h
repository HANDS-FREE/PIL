#ifndef PIL_TestMain_H
#define PIL_TestMain_H
#include "TestCase.h"

int main(int argc,char** argv)
{
    svar.ParseMain(argc,argv);
    pi::TestCase test;
    test.callAll();
    std::cerr<<test.generateReport();
    return 0;
}
#endif // TESTMAIN_H
