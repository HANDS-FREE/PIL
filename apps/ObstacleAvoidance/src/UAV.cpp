#include "UAV.h"
#include <base/Svar/Svar.h>
#include <base/Svar/Scommand.h>
#include <base/Types/SE3.h>
#include <base/Time/Global_Timer.h>
#include <base/Time/Time.h>
#include <base/Types/Random.h>

#include <gui/gl/glHelper.h>

#define PI          (3.1415926)
#define D2R         (3.1415926 / 180.0)
#define R2D         (180.0/3.1415926)
#define M2FT        (0.3048)                //transfer (m/sec^2) to (ft/sec^2)
#define R_EARTH     (6378100.0)
#define G_GROUND    (9.8)

using namespace std;

class UAVImpl
{
    friend class UAV;
public:
    UAVImpl(const string& uavName);
    ~UAVImpl(){}

    virtual bool setTarget(const pi::Point3d& target,bool cameraCoordinate=true){return false;}

    virtual bool setRemote(const double& roll,const double& pitch,const double& thrust,const double& yaw){return false;}

    virtual std::string Type(){return "NoUAV";}

    pi::SE3d getPose(){pi::ReadMutex lock(mutexPose);return CameraToBoard;}
    void     setPose(const pi::SE3d& pose){pi::WriteMutex lock(mutexPose);CameraToBoard=pose;}

    bool valid(){return state>=0;}

    virtual void draw();
protected:
    string           name;
    pi::SE3d         CameraToBoard,a,v;
    pi::MutexRW      mutexPose;
    int              state;
    float            uavSize;

    int&             targetYawSetted;
    float            targetYaw;
};

class UAVVirtual:public UAVImpl,public pi::Thread
{
public:
    UAVVirtual(const string& uavName);
    ~UAVVirtual();
    virtual bool setTarget(const pi::Point3d &target, bool cameraCoordinate=true);
    virtual bool setRemote(const double& roll,const double& pitch,const double& thrust,const double& yaw);

    virtual std::string Type(){return "Virtual";}

    virtual void draw();

private:
    void    run();
    bool    updateRemoteFromTarget();
    bool    updateStateFromRemote();

    // remote state
    pi::Point3d targetW;
    bool        targetSeted;
    double jsR,jsP,jsT,jsY,&jsDeadZone;//remote values
    double& maxVelocity,&minVelocity;
    double& MaxRotationStep;
    pi::MutexRW mutexRemote;

    // sim
    double &sim_dragK,windMax;
    pi::Point3d windVelo,windRand;

    double sim_tNow,sim_tLast;//simulate time
};

class UAVINNNO:public UAVImpl
{
public:
    UAVINNNO(const string& uavName):UAVImpl(uavName){}
    virtual bool setTarget(const pi::Point3d &target, bool cameraCoordinate=true){return false;}
    virtual bool setRemote(const double& roll,const double& pitch,const double& thrust,const double& yaw){return false;}

    virtual std::string Type(){return "INNNO";}

};


UAVImpl::UAVImpl(const string& uavName)
    : name(uavName),state(-1),
      uavSize(svar.GetDouble(uavName+".Size",0.2)),
      targetYawSetted(svar.GetInt(uavName+".TargetYawSetted")),
      targetYaw(0)
{
}

void UAVImpl::draw()
{
    if(!svar.GetInt("GlutInited"))
    {
        glutInit(&svar.i["argc"],SvarWithType<char**>::instance()["argv"]);
        svar.GetInt("GlutInited")=1;
    }
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);

    glMultMatrix(getPose());
    if(state<0) return;

    // draw the uav here
    const float length=0.3;
    glDisable(GL_LIGHTING);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glColor3ub(255,0,0);glVertex3f(0,0,0);glVertex3f(length,0,0);
    glColor3ub(0,255,0);glVertex3f(0,0,0);glVertex3f(0,length,0);
    glColor3ub(0,0,255);glVertex3f(0,0,0);glVertex3f(0,0,length);
    glEnd();

    // consider to draw the wind, this may be cool and funny, haha~
    glLineWidth(5);
    glBegin(GL_LINES);
    glColor3ub(0,255,0);glVertex3f(0.5*uavSize,0.5*uavSize,0);glVertex3f(-0.5*uavSize,-0.5*uavSize,0);
    glColor3ub(0,255,0);glVertex3f(-0.5*uavSize,0.5*uavSize,0);glVertex3f(0.5*uavSize,-0.5*uavSize,0);
    glEnd();
    glColor3ub(255,0,0); glTranslatef(0.5*uavSize,0.5*uavSize,0); glutSolidCone(0.22*uavSize,0.05*uavSize,30,2);
    glColor3ub(0,0,255); glTranslatef(-uavSize,0,0); glutSolidCone(0.22*uavSize,0.05*uavSize,30,2);
    glColor3ub(0,0,255); glTranslatef(0,-uavSize,0); glutSolidCone(0.22*uavSize,0.05*uavSize,30,2);
    glColor3ub(255,0,0); glTranslatef(uavSize,0,0);  glutSolidCone(0.22*uavSize,0.05*uavSize,30,2);

    glPopMatrix();
}

void UAVVirtual::draw()
{
    UAVImpl::draw();
    pi::SE3d pose=getPose();
    glColor3ub(0,255,255);
    glBegin(GL_LINES);
    glVertex(pose.get_translation());
    {
        pi::ReadMutex lock(mutexRemote);
        glVertex(targetW);
    }
    glEnd();
}

UAVVirtual::UAVVirtual(const string& uavName)
    : UAVImpl(uavName),
      targetW(svar.get_var(uavName+".InitTarget",pi::Point3d(0,0,1))),
      targetSeted(true),
      jsDeadZone(svar.GetDouble(uavName+".DeadZone",0.001)),
      maxVelocity(svar.GetDouble(uavName+".MaxVelocity",10)),
      minVelocity(svar.GetDouble(uavName+".MinVelocity",0.3)),
      MaxRotationStep(svar.GetDouble(uavName+".MaxRotationStep",0.005)),
      sim_dragK (svar.GetDouble(uavName+".DragK" , 0.25)),
      windMax(svar.GetDouble(uavName+".WindMax",1.)),
      windRand(svar.get_var(uavName+".WindRand",pi::Point3d(0.05,0.05,0.002))),
      sim_tNow(0),sim_tLast(0)
{
    state=0;
    start();
}

UAVVirtual::~UAVVirtual()
{

}

bool UAVVirtual::setTarget(const pi::Point3d &target, bool cameraCoordinate)
{
    pi::WriteMutex lock(mutexRemote);
    if(cameraCoordinate)
    {
        targetW=CameraToBoard*target;
    }
    else targetW=target;
    targetSeted=true;
    targetYawSetted=false;
    return true;
}

bool UAVVirtual::setRemote(const double& roll,const double& pitch,const double& thrust,const double& yaw)
{
    pi::WriteMutex lock(mutexRemote);
    jsR=roll;jsP=pitch;jsT=thrust;jsY=yaw;
    return true;
}

void UAVVirtual::run()
{
    pi::Rate rate(svar.GetInt("UAVVirtual.Rate",100));
//    int& pause=svar.GetInt("Pause");
    while(!shouldStop())
    {
        if(targetSeted) updateRemoteFromTarget();
        updateStateFromRemote();
        rate.sleep();
    }
}

bool UAVVirtual::updateRemoteFromTarget()
{
    stringstream    info;
    pi::SE3d        pose=getPose();
    pi::Point3d     motionNeeded=targetW-pose.get_translation();
    double          distance=motionNeeded.norm();

    // 1. Obtain idea velocity and accelleration
    pi::Point3d     velocityNeeded=motionNeeded;
    if(distance>maxVelocity)
        velocityNeeded=velocityNeeded*(maxVelocity/distance);
//    else velocityNeeded=velocityNeeded*sqrt(maxVelocity/distance);
//    else if(distance<minVelocity)
//    {
//        velocityNeeded=velocityNeeded*(minVelocity/distance);
//    }

    pi::Point3d& V_earth    = v.get_translation();//velocity
    pi::Point3d  accNeeded  = 2*(velocityNeeded-V_earth);
    if(accNeeded.z<-5) accNeeded.z=-5;
    // FIXME: could accelleration change very fast?
    // pi::Point3d accChange=accNeeded-a.get_translation();

    // 2. Compute idea thrust of UAV (consideration of gravity and air friction)
    // FIXME: air with wind friction estimation, should we consider it?
    pi::Point3d D_earth ;//= -V_earth * V_earth.norm()*sim_dragK;
    pi::Point3d G_earth = pi::Point3d(0, 0, -G_GROUND);         //gravity
    pi::Point3d ThrustNeeded = accNeeded-(D_earth+G_earth);
    pi::Point3d axisZneeded = ThrustNeeded.normalize();
    if(axisZneeded.z<0) return false;//never drop down!!
    if(axisZneeded.z<cos(D2R*20))//should no more than 30 degree
    {
        axisZneeded=pi::Point3d(axisZneeded.x*axisZneeded.z,axisZneeded.y*axisZneeded.z,axisZneeded.z).normalize();
    }

    // 3. Compute next rotation
    const pi::SO3d& curR =pose.get_rotation();
    pi::SO3d        nextR;
    // 3.1. Compute the target rotation according to thrust
    {
        if(distance>1&&!targetYawSetted)//follow target yaw
        {
            pi::Point3d vecXYaw=motionNeeded;
            pi::Point3d axisY=axisZneeded.cross(vecXYaw).normalize();
            pi::Point3d axisX=axisY.cross(axisZneeded).normalize();
            pi::Point3d axisZ=axisZneeded.normalize();
            double R[9];
            R[0]=axisX.x;R[1]=axisY.x;R[2]=axisZ.x;
            R[3]=axisX.y;R[4]=axisY.y;R[5]=axisZ.y;
            R[6]=axisX.z;R[7]=axisY.z;R[8]=axisZ.z;
            nextR.fromMatrix(R);
            targetYaw=nextR.getYaw();
            targetYawSetted=true;
        }
        else
        {
            if(!targetYawSetted)
            {
                targetYaw=curR.getYaw();
            }
            nextR.FromEuler(0,targetYaw,0);
            double R[9];
            nextR.getMatrix(R);
            pi::Point3d vecXYaw(R[0],R[3],R[6]);
            pi::Point3d axisY=axisZneeded.cross(vecXYaw).normalize();
            pi::Point3d axisX=axisY.cross(axisZneeded).normalize();
            pi::Point3d axisZ=axisZneeded;
            R[0]=axisX.x;R[1]=axisY.x;R[2]=axisZ.x;
            R[3]=axisX.y;R[4]=axisY.y;R[5]=axisZ.y;
            R[6]=axisX.z;R[7]=axisY.z;R[8]=axisZ.z;
            nextR.fromMatrix(R);
//            cout<<"axis:"<<axisX<<endl<<axisY<<endl<<axisZ<<endl;
//            cout<<"NowYaw:="<<nextR.getYaw()<<endl;
        }
    }

    // 3.2 Do rotation smooth
    if(MaxRotationStep>0)
    {
        pi::SO3d lastRd=v.get_rotation();
        pi::Point3d lastRdln=lastRd.ln();
        pi::SO3d Rd=nextR*curR.inv();
        pi::Point3d Rdln=Rd.ln();
        double Rdnorm=(Rdln-lastRdln).norm();
        if(Rdnorm>MaxRotationStep)
        {
            Rdln=lastRdln+(Rdln-lastRdln).normalize()*MaxRotationStep;
            Rd=pi::SO3d::exp(Rdln*(MaxRotationStep/Rdnorm));
            nextR=Rd*curR;
        }
    }

    // 4. Determination of thrust, the real thrust uav can take, how to decide this?
    pi::Point3d ThrustApply = ThrustNeeded;
    double      ThrustNum   = ThrustNeeded.norm();
    if(1)//ensure the same z
    {
        ThrustNum=ThrustNeeded.z/axisZneeded.z;
    }
    else
    {
        if(ThrustNum>2*G_GROUND)
            ThrustNum=2*G_GROUND;
        ThrustApply=axisZneeded*ThrustNum;// most easy, give all what you want
    }

    info<<"V_earth:"<<V_earth<<",accNeeded:"<<accNeeded<<",wind:"<<windVelo
       <<",ThrustNeeded:"<<ThrustNeeded<<",ThrustApply:"<<ThrustApply<<endl;

    setRemote(nextR.getRoll(),nextR.getPitch(),ThrustNum/G_GROUND,nextR.getYaw());

    return true;
}

bool UAVVirtual::updateStateFromRemote()
{
    stringstream info;

    info<<"Remote R:"<<jsR*R2D<<",P:"<<jsP*R2D<<",T:"<<jsT*R2D<<",Y:"<<jsY*R2D<<endl;
    double      dt;
    // time
    sim_tNow = pi::Timestamp().timestampF();
    dt = sim_tNow - sim_tLast;
    if( dt > 100 ) {
        sim_tLast = sim_tNow;//first time
        return -1;
    }
    sim_tLast = sim_tNow;

    // process joystick values
    if( jsR > -jsDeadZone && jsR < jsDeadZone ) jsR = 0;
    if( jsP > -jsDeadZone && jsP < jsDeadZone ) jsP = 0;
    if( jsT > -jsDeadZone && jsT < jsDeadZone ) jsT = 0;
    if( jsY > -jsDeadZone && jsY < jsDeadZone ) jsY = 0;


    // calculate roll, pitch, yaw
    // rotation matrix of Euler
    // FIXME: angle need to be processed
    pi::SE3d  lastPose=getPose();
    pi::SO3d& lastR   =lastPose.get_rotation();
    double _yaw, _roll, _pitch;

    _roll  = jsR;//30.0 * jsR * D2R;
    _pitch = jsP;//30.0 * jsP * D2R;
//    _yaw   = lastR.getYaw() * D2R;
    _yaw   = jsY;//lastR.getYaw()+jsY*dt;//30.0 * jsY * D2R*dt;


    pi::SE3d  curPose=lastPose;
    pi::SO3d& curR   =curPose.get_rotation();
    curR.FromEuler(_pitch, _yaw, _roll);

    //Update velocity and position
    pi::Point3d& V_earth=v.get_translation();//last velocity
    v.get_rotation()=curR*lastR.inv();

    windVelo=windVelo+pi::Point3d(pi::Random::RandomGaussianValue(0.,windRand.x),
                                  pi::Random::RandomGaussianValue(0.,windRand.y),
                                  pi::Random::RandomGaussianValue(0.,windRand.z));
    double windScale=windVelo.norm();
    if(windScale>windMax) windVelo=windVelo*(windMax/windScale);
    pi::Point3d V_air   = (V_earth+windVelo);
    pi::Point3d D_earth = -V_air * V_air.norm()*sim_dragK;  //friction from air
    pi::Point3d G_earth = pi::Point3d(0, 0, -G_GROUND);         //gravity
    pi::Point3d T_earth = curR * pi::Point3d(0.0,0.0,double(G_GROUND*(jsT)));//thrust

    // calc  A_earth
    pi::Point3d A_earth = (T_earth + G_earth + D_earth);//D_earth: friction from air, G_earth: gravity, T_earth: thrust
    a.get_translation()=A_earth;
    info<<"T:"<<T_earth<<",G:"<<G_earth<<",D:"<<D_earth<<",A:"<<A_earth<<endl;

    // calc  V_earth
    V_earth  = V_earth + A_earth*dt;
//    double v_norm=V_earth.norm();
//    if(v_norm>10)// too fast
//        V_earth=V_earth*(1./v_norm);

    // calculate x,y,z offset in earth frame
    pi::Point3d& position=curPose.get_translation();
    position=lastPose.get_translation()+V_earth*dt;
    // FIXME: need DEM data to prevent below ground
    if(position.z < 0 ) position.z = 0;
    setPose(curPose);
    info<<"V_earth:"<<V_earth<<",Pose:"<<curPose<<endl;
//    cout<<info.str()<<endl;
    return true;
}


void UAVHandle(void* ptr, std::string sCommand, std::string sParams)
{
    UAV* uav=(UAV*)ptr;
    string cmd;
    stringstream para(sParams);
    para>>cmd;
    if(cmd=="setTarget")
    {
        pi::Point3d target;
        para>>target.x>>target.y>>target.z;
        cout<<"Setting world target "<<target<<endl;
        uav->setTarget(target,false);
    }
    else if(cmd=="setCameraTarget")
    {
        pi::Point3d target;
        para>>target.x>>target.y>>target.z;
        cout<<"Setting camera target "<<target<<endl;
        uav->setTarget(target,true);
    }
    else if(cmd=="setState")
    {
        int mode;
        para>>mode;
        uav->setState(mode);
    }
}

UAV::UAV(const std::string& uav_name):impl(new UAVImpl(uav_name))
{
    string type=svar.GetString(uav_name+".Type","NoUAV");
    if(type=="Virtual") impl=SPtr<UAVImpl>(new UAVVirtual(uav_name));
    else if(type=="INNNO") impl=SPtr<UAVImpl>(new UAVINNNO(uav_name));
    cout<<"UAV: "<<uav_name<<" created "<<((impl->valid())?"Success.":"Failed.");
    if(impl->valid())
    {
        scommand.RegisterCommand(uav_name,UAVHandle,this);
    }
}


void UAV::draw()
{
    impl->draw();
}

bool UAV::setTarget(const pi::Point3d& target,bool cameraCoordinate)
{
    return impl->setTarget(target,cameraCoordinate);
}

bool UAV::setRemote(const double &roll, const double &pitch, const double &thrust, const double &yaw)
{
    return impl->setRemote(roll,pitch,thrust,yaw);
}

std::string  UAV::type()
{
    return impl->Type();
}

bool         UAV::valid()
{
    return impl->valid();
}

int          UAV::state()
{
    return impl->state;
}

pi::SE3d     UAV::getPose()
{
    return impl->getPose();
}

bool  UAV::setState(int state)
{
    impl->state=state;
    return true;
}
