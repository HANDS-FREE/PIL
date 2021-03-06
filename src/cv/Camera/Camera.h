#ifndef CAMERAS_H
#define CAMERAS_H

#include <base/Types/Point.h>
#include <base/Types/SPtr.h>

namespace pi {

class CameraImpl;

class Camera
{
public:
    Camera(const std::string& name="");
    Camera(SPtr<CameraImpl>& Impl);

    static Camera createFromName(const std::string& name);

    std::string CameraType();

    std::string info();

    bool applyScale(double scale=0.5);

    bool isValid();

    Point2d Project(const Point3d& p3d);

    Point3d UnProject(const Point2d& p2d);

    int width();

    int height();
private:
    SPtr<CameraImpl> impl;
};

}

#endif // CAMERAS_H
