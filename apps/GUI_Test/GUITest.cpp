#include <base/Svar/Svar.h>
#include <gui/gl/Win3D.h>
#include <base/Thread/Thread.h>
#include <gui/gl/SignalHandle.h>
#include <gui/widgets/SvarTable.h>
#include <gui/gl/glHelper.h>

using namespace std;

class System : public pi::Thread, public pi::gl::EventHandle,public pi::gl::Draw_Opengl
{
public:
    System(){
        win3d.Show();
        start();
        cout<<"Started!\n";
    }

    ~System()
    {
        stop();
        while(isRunning()) sleep(10);
    }

    virtual void run()
    {
        win3d.SetEventHandle(this);
        win3d.SetDraw_Opengl(this);
        int i=0;
        while(!shouldStop())
        {
            cout<<i++<<"Seconds!\n";
            sleep(1000);
        }
        cout<<"Thread stoped.\n";
    }


    virtual bool KeyPressHandle(void *arg) {
        QKeyEvent* e=(QKeyEvent*)arg;
        switch(e->key())
        {
        case Qt::Key_S:
        {
            SvarWithType<SvarWidget*>& inst=SvarWithType<SvarWidget*>::instance();
            if(!inst.exist("SvarWidget"))
            {
                inst["SvarWidget"]=new SvarWidget();
            }
            inst["SvarWidget"]->show();
            return true;
        }

        default:
            svar.i["KeyPressMsg"] = e->key();
            return false;
        }
    }

    virtual void Draw_Something()
    {
        glDisable(GL_LIGHTING);
        glLineWidth(2.5);
        glBegin(GL_LINES);
        glColor(pi::Point3ub(255,0,0));
        glVertex(pi::Point3d());
        glVertex(pi::Point3d(1,1,1));
        glEnd();
    }

    pi::gl::Win3D win3d;
};

int main(int argc,char** argv)
{
    QApplication qapp(argc,argv);
    svar.ParseMain(argc,argv);
    int ret=0;
    try{
        System system;
        ret=qapp.exec();
    }
    catch(pi::Exception e){
        cout<<e.displayText()<<endl;
    }

    return ret;
}
