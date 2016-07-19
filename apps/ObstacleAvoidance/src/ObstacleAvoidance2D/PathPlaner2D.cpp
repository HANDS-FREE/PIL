#include <base/time/Global_Timer.h>

#include "PathPlaner2D.h"

class PathPlaner2DImpl
{
public:
    ~PathPlaner2DImpl(){}
    virtual bool feedMap(const SPtr<Map2DFrame>& frame){return false;}
    virtual void setStart(const cv::Point3f& position){}
    virtual void setEnd(const cv::Point3f&   position){}
    virtual bool obtainPath(std::vector<cv::Point2f>& path){return false;}
};

PathPlaner2D::PathPlaner2D(PathPlaner2DType type)
    :impl(new PathPlaner2DImpl())
{

}


bool PathPlaner2D::feedMap(const SPtr<Map2DFrame>& frame)
{
    return impl->feedMap(frame);
}

void PathPlaner2D::setStart(const cv::Point3f& position)
{
    return impl->setStart(position);
}

void PathPlaner2D::setEnd(const cv::Point3f&   position)
{
    return impl->setEnd(position);
}

bool PathPlaner2D::obtainPath(std::vector<cv::Point2f>& path)
{
    return impl->obtainPath(path);
}
