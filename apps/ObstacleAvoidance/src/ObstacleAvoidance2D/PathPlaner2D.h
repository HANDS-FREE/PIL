#ifndef PATHPLANER2D_H
#define PATHPLANER2D_H
#include <opencv2/core/core.hpp>
#include <base/types/SPtr.h>

struct Map2DFrame
{
    cv::Mat     occupy;
    cv::Point2f position;
    float       gridSize;
    // sonar
    // current pose
};

enum PathPlaner2DType
{
    PathPlaner2DTypeNone=0,
    PathPlaner2DTypeAStar=1
};

class PathPlaner2DImpl;
class PathPlaner2D
{
public:
    PathPlaner2D(PathPlaner2DType type=PathPlaner2DTypeNone);

    bool feedMap(const SPtr<Map2DFrame>& frame);
    void setStart(const cv::Point3f& position);
    void setEnd(const cv::Point3f&   position);
    bool obtainPath(std::vector<cv::Point2f>& path);

private:
    SPtr<PathPlaner2DImpl> impl;
};

#endif // PATHPLANER2D_H
