#include <base/Svar/Svar.h>
#include <gui/gl/Win3D.h>
#include <base/Thread/Thread.h>
#include <gui/gl/SignalHandle.h>
using namespace std;

class System : public pi::Thread, public pi::gl::EventHandle
{
public:
    System(){
        win3d.Show();
        start();
    }

    ~System()
    {
        stop();
        while(isRunning()) sleep(10);
    }

    virtual void run()
    {
        int i=0;
        while(!shouldStop())
        {
            cout<<i<<"Seconds!\n";
            sleep(1000);
        }
    }

    pi::gl::Win3D win3d;
};

int main(int argc,char** argv)
{
    QApplication qapp(argc,argv);
    svar.ParseMain(argc,argv);
    System system;

    return qapp.exec();
}
