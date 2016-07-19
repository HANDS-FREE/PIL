#include <base/Thread/Thread.h>
#include <base/Svar/Scommand.h>
#include <base/Time/Global_Timer.h>
#include <base/Time/DateTime.h>
#include <gui/gl/Win3D.h>
#include <gui/gl/PosedObject.h>
#include <cv/Camera/Camera.h>

#include "System.h"
#include "MainWindow.h"
#include "UAV.h"
#include "GLScence.h"
#include "ObstacleAvoidance.h"

using namespace std;

class SystemImpl:public pi::Thread,public pi::gl::Draw_Opengl,public pi::gl::EventHandle
{
public:
    SystemImpl()
        : uav(svar.GetString("UAV.Name","VirtualUAV")),
          pause(svar.GetInt("Pause",0)),
          cameraFollow(svar.GetInt("CameraFollow",0))
    {
        cout.rdbuf( info.rdbuf() );
        start();
    }


    virtual void run();
    virtual void Draw_Something();
    virtual bool KeyPressHandle(void* arg);
    virtual bool MousePressHandle(void *);

    void TestObstacleAvoidance();

private:
    UAV                       uav;
    SPtr<ObstacleAvoidance>   obstacleAvoidance;

    pi::Camera      camera;

    MainWindow                mainWindow;

    stringstream              info;
    string                    act;

    int&                      pause;
    int&                      cameraFollow;
};

void SystemImpl::run()
{
    act=svar.GetString("Act","ObstacleAvoidance");
    if(act=="ObstacleAvoidance") TestObstacleAvoidance();
    else if(act=="") TestObstacleAvoidance();
    else cerr<<"No such action!\n";
    stop();
}

void SystemImpl::TestObstacleAvoidance()
{
    pi::Rate rate(svar.GetInt("MainRate",30));
    mainWindow.call("show");
    mainWindow.getWin3D()->SetDraw_Opengl(this);
    mainWindow.getWin3D()->SetEventHandle(this);

    pi::SE3d camera2uav;
    camera2uav.get_rotation()=pi::SO3d::FromAxis(pi::Point3d(0,0,1),-3.1415926*0.5)*pi::SO3d::FromAxis(pi::Point3d(1,0,0),-3.1415926*0.5);
    obstacleAvoidance=SPtr<ObstacleAvoidance>(new ObstacleAvoidance(uav,svar.GetInt("ObstacleAvoidance.Type",0)));

    pi::TicTac tictac;
    double Win3DUpdateT=svar.GetDouble("Win3DUpdataT",0.033);
    while(!shouldStop())
    {
        pi::timer.enter("MainLoop");
        info.str("");

        // 1. Update camera pose and flush
        stringstream sst;
        pi::SE3d uavPose=uav.getPose();
        pi::SE3d cameraPose=uavPose*camera2uav;
        sst<<"SetCameraPose "<<cameraPose;
        mainWindow.call(sst.str());
        info<<"UAVPose:"<<uavPose<<endl;


        // 2. Set
        if(obstacleAvoidance.get())
        {
            double timestamp;
            pi::timer.enter("ObtainDepth");
            cv::Mat depth=mainWindow.getSimCamera()->obtainDepth(timestamp);
            pi::timer.leave("ObtainDepth");
            if(timestamp)
            {
                SPtr<InputFrame> frame(new InputFrame);
                frame->camera=camera;
                frame->depth=depth;
                frame->pose=uavPose;
                frame->timestamp=timestamp;
                obstacleAvoidance->feedFrame(frame);
            }
        }

        if(tictac.Tac()>Win3DUpdateT)
        {
            pi::Point3d position=uavPose.get_translation();
            mainWindow.getWin3D()->setSceneCenter(qglviewer::Vec(position.x,position.y,position.z));
            mainWindow.getSimCamera()->update();
            mainWindow.getWin3D()->update();
            SvarWithType<QString>::instance().insert("SystemInfo",QString::fromStdString(info.str()),true);
            mainWindow.call("UpdateText");
            tictac.Tic();
        }

        pi::timer.leave("MainLoop");
        rate.sleep();
    }
}

void SystemImpl::Draw_Something()
{
    qglviewer::Vec v=mainWindow.getWin3D()->camera()->position();
    mainWindow.getScence()->setViewPoint(v.x,v.y,v.z);
    mainWindow.getScence()->draw();
    uav.draw();
}

bool SystemImpl::KeyPressHandle(void *arg) {
    QKeyEvent* e=(QKeyEvent*)arg;
    switch(e->key())
    {
    case Qt::Key_Space:
        return false;

    case Qt::Key_Escape:
        stop();
        return false;

    case Qt::Key_P:
        pause=!pause;
        return true;

    default:
        svar.i["KeyPressMsg"] = e->key();
        return false;
    }
}

bool SystemImpl::MousePressHandle(void *arg)
{
    QMouseEvent* e=(QMouseEvent*)arg;
    if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ControlModifier))
    {
        qglviewer::Vec target;
        bool           found;
        target=mainWindow.getWin3D()->camera()->pointUnderPixel(e->pos(),found);
        if(found)
        {
            if(obstacleAvoidance.get())
            {
                pi::SE3d  tgt(pi::SO3d(),pi::Point3d(target.x,target.y,3));
                obstacleAvoidance->setTargetPose(tgt);
            }
            else
            {
                stringstream cmd;
                cmd<<"VirtualUAV setTarget "<<target.x<<" "<<target.y<<" "<<3;
                svar.ParseLine(cmd.str());
            }
        }
    }
    return false;
}

System::System():impl(new SystemImpl())
{

}

void System::stop()
{
    impl->stop();
    impl->join();
    if(impl->isRunning()) usleep(1000);
    cout<<"System joined.\n";
}
