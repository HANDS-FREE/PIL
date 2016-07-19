#ifndef UAV_H
#define UAV_H

#include <base/types/types.h>
#include <base/types/SPtr.h>
#include <base/types/SE3.h>

class UAVImpl;

class UAV
{
public:
    UAV(const std::string& uav_name);
    bool setTarget(const pi::Point3d& target,bool cameraCoordinate=true);
    bool setRemote(const double& roll,const double& pitch,const double& thrust,const double& yaw);
    void draw();

    std::string  type();
    bool         valid();
    int          state();
    bool         setState(int state);
    pi::SE3d     getPose();

private:
    SPtr<UAVImpl> impl;
};

#endif // UAV_H
