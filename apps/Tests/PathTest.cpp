
#include <base/Path/Path.h>
#include <base/Utils/TestCase.h>
using namespace pi;

class PathTest : public pi::TestCase
{
public:
    PathTest():pi::TestCase("PathTest"){}

    virtual void run()
    {
        testAbsolutePath();
    }

    void testAbsolutePath()
    {
        pi::Path abs("/data/zhaoyong/Linux/Program/Apps/PIL-1.1.0/Default.cfg");
        pi_assert(abs.isAbsolute()==true);
        pi_assert(abs.getBaseName()=="Default");
        pi_assert(abs.getDevice()=="");
        pi_assert(abs.getExtension()=="cfg");
        pi_assert(abs.getFileName()=="Default.cfg");
        pi_assert(abs.getFolderName()=="PIL-1.1.0");
        pi_assert(abs.getFolderPath()=="/data/zhaoyong/Linux/Program/Apps/PIL-1.1.0");
    }
};
PathTest pathTest;
