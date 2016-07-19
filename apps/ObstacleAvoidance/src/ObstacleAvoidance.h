#ifndef OBSTACLEAVOIDANCE_H
#define OBSTACLEAVOIDANCE_H
#include <deque>
#include <opencv2/core/core.hpp>

#include <cv/Camera/Camera.h>

#include "UAV.h"

struct SonarData
{
    pi::Point3f direction;
    double      distance;
};

struct InputFrame
{
    double                  timestamp;
    cv::Mat                 img,depth;
    pi::Camera    camera;
    pi::SE3d                pose;
    SPtr<std::vector<SonarData> >  sonarData;
};

class ObstacleAvoidanceImpl
{
public:
    ObstacleAvoidanceImpl(const UAV& _uav)
        :uav(_uav){}

    virtual bool feedFrame(const SPtr<InputFrame>& frame){
        frames.push_back(frame);
        while(frames.size()>2) frames.pop_front();
        return true;
    }

    virtual bool setTargetPose(const pi::SE3d& target){uav.setTarget(target.get_translation(),false);return true;}

    virtual void draw(){}

    UAV                           uav;
    std::deque<SPtr<InputFrame> > frames;
};

enum ObstacleAvoidanceType
{
    ObstacleAvoidanceTypeNone=0,
    ObstacleAvoidanceType2D=1
};

class ObstacleAvoidance
{
public:
    ObstacleAvoidance(const UAV& _uav,int type=0);
    bool feedFrame(const SPtr<InputFrame>& frame);
    bool setTargetPose(const pi::SE3d& target);

    void draw();

private:
    SPtr<ObstacleAvoidanceImpl> impl;
};

#endif // OBSTACLEAVOIDANCE_H
