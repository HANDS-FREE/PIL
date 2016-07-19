#ifndef VIDEOCOMPARE_H
#define VIDEOCOMPARE_H
#include "VideoRef.h"
#include "ORBextractor.h"

class VideoCompare
{
public:
    VideoCompare(VideoRef& ref);


    int DescriptorDistance(const cv::Mat &a, const cv::Mat &b);
    inline cv::Point2d mult(double* H,const cv::Point2d& pt);

    bool checkFrame(SPtr<VideoFrame>& frame);
    bool trackFrame(SPtr<VideoFrame>& frame,SPtr<VideoFrame>& ref,cv::Mat initH);
    bool trackFrame(SPtr<VideoFrame>& frame,SPtr<VideoFrame>& ref);
    bool train(SPtr<VideoFrame>& frame);// prepare the videoRef
    bool track(SPtr<VideoFrame>& frame);// track the videoRef and obtain the Homogragh matrix

    float sharedPercent(int w,int h,cv::Mat H);

    VideoRef& videoRef;

    SPtr<VideoFrame> lastKeyframe;
    ORBextractor     extractor;
    cv::Mat          H2Last;

    // things to publish
    cv::Mat          &refImg,&refImgHere,&trackImg,&warpImg,&diffImg;

};

inline cv::Point2d VideoCompare::mult(double* M,const cv::Point2d& pt)
{
    double X0 = M[0]*pt.x + M[1]*pt.y + M[2];
    double Y0 = M[3]*pt.x + M[4]*pt.y + M[5];
    double W0 = 1./(M[6]*pt.x + M[7]*pt.y + M[8]);
    return cv::Point2d(X0*W0,Y0*W0);
}

#endif // VIDEOCOMPARE_H
