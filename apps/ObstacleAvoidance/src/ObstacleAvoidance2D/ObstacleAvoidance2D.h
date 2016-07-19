#ifndef OBSTACLEAVOIDANCE2D_H
#define OBSTACLEAVOIDANCE2D_H
#include "../ObstacleAvoidance.h"
#include "PathPlaner2D.h"
#include "Mapper2D.h"

class ObstacleAvoidance2D:public ObstacleAvoidanceImpl
{
public:
    ObstacleAvoidance2D(const UAV& _uav);

    Mapper2D      mapper;
    PathPlaner2D  pathPlaning;
};

#endif // OBSTACLEAVOIDANCE2D_H
