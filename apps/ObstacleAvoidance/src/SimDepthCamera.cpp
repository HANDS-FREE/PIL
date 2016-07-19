#include "SimDepthCamera.h"
#include <base/Svar/Svar.h>
#include <base/time/Time.h>
#include <base/time/Global_Timer.h>


SimCamera::SimCamera(QWidget *parent)
    :pi::gl::Win3D(parent)
{

}

void SimCamera::initializeGL()
{
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Default colors
    setForegroundColor(QColor(svar.GetString("Win3D.ForegroundColor","#B4B4B4").c_str()));

    // Clear the buffer where we're going to draw
    if (format().stereo())
    {
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calls user defined method. Default emits a signal.
    init();

    // Give time to glInit to finish and then call setFullScreen().
    if (isFullScreen())
        QTimer::singleShot( 100, this, SLOT(delayedFullScreen()) );
}


void SimCamera::draw()
{
    pi::timer.enter("SimCamera::draw");
    pi::gl::Win3D::draw();

    glFlush();
//    double timestamp=pi::Timestamp().timestampF();
    cv::Mat depthMat(this->height(),this->width(),CV_32F);
    glReadPixels(0, 0, this->width(), this->height(), GL_DEPTH_COMPONENT, GL_FLOAT, depthMat.data);

    //convert to real world distance
    if(0)
    {
//        float near,far,left,right,bottom,top;
//        near=camera()->zNear();
//        far=camera()->zFar();
//        left=-cx/fx*near;
//        right=(width()-cx)/fx*near;
//        bottom=-(height()-cy)/fy*near;
//        top=(cy)/fy*near;
//        for(int i = 0; i < width()*height(); ++i)
//        {
//            float& z_b = depthMat.at<float>(i);
//            float z_n = 2.0f * z_b - 1.0f;
//            z_b = 2.0 * near * far / (far + near - z_n * (far - near));
//        }
    }

    {
        cv::Mat depthuchar;
        depthMat.convertTo(depthuchar,CV_8UC1,255.);


        // FIXME: up/down swap
        if( 1 ) {
            cv::Mat depthucharclone=depthuchar.clone();
            uchar *p1, *p2;
            int   step=depthuchar.cols;
            int   h=depthuchar.rows;
            p1 = depthuchar.data;
            p2 = depthucharclone.data + (h-1)*step;

            for(int j=0,end=h; j<end; j++) {
                memcpy(p2, p1, step);

                p1 += step;
                p2 -= step;
            }
            depthuchar=depthucharclone;
        }

        QImage        qimage(depthuchar.data,depthuchar.cols,depthuchar.rows,QImage::Format_Indexed8);
        depthImage->setPixmap(QPixmap::fromImage(qimage));
    }

//    {
//        pi::WriteMutex lock(depthMutex);
//        depth=depthMat;
//        depthTimestamp=timestamp;
//    }

    pi::timer.leave("SimCamera::draw");
}

cv::Mat SimCamera::obtainDepth(double &timestamp){
    pi::ReadMutex lock(depthMutex);
    timestamp=depthTimestamp;
    depthTimestamp=0;
    return depth.clone();
}
