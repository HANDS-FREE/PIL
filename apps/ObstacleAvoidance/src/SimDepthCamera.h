#ifndef SIMDEPTHCAMERA_H
#define SIMDEPTHCAMERA_H
#include <QLabel>
#include <opencv2/highgui/highgui.hpp>

#include <gui/gl/Win3D.h>

class SimCamera:public pi::gl::Win3D
{
public:
    SimCamera(QWidget *parent=NULL);

    virtual void initializeGL();

    virtual void draw();

    cv::Mat obtainDepth(double &timestamp);

    QLabel*     depthImage;
private:
    cv::Mat     depth;
    double      depthTimestamp;
    pi::MutexRW depthMutex;
};
#endif // SIMDEPTHCAMERA_H
