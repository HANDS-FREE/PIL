#include "ObstacleAvoidance.h"
#include "ObstacleAvoidance2D/ObstacleAvoidance2D.h"

ObstacleAvoidance::ObstacleAvoidance(const UAV& _uav,int type):
    impl(new ObstacleAvoidanceImpl(_uav))
{
    if(type==ObstacleAvoidanceType2D) impl=SPtr<ObstacleAvoidanceImpl>(new ObstacleAvoidance2D(_uav));
}

bool ObstacleAvoidance::feedFrame(const SPtr<InputFrame>& frame)
{
    return impl->feedFrame(frame);
}

bool ObstacleAvoidance::setTargetPose(const pi::SE3d& target)
{
    return impl->setTargetPose(target);
}

void ObstacleAvoidance::draw()
{
    return impl->draw();
}
