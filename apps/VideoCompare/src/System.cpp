#include "System.h"
#include <base/Svar/Svar.h>
#include <opencv2/highgui/highgui.hpp>
#include "VideoCompare.h"
#include "VideoFrame.h"
#include <base/Time/Global_Timer.h>

System::System()
{
}

System::~System()
{
    stop();
    if(isRunning())
    {
        sleep(10);
    }
    join();
}

void System::run()
{
    std::string& refVideoPath=svar.GetString("VideoRef.VideoPath","");
    if(!refVideoPath.size()){
        cerr<<"Please set the \"refVideoPath\"!\n";
        return;
    }

    // load vocabulary
    string vocFile=svar.GetString("ORBVocabularyFile","");
    if(vocFile.empty())
    {
        cerr<<"Please set 'ORBVocabularyFile'.\n";
        return;
    }

    cout <<"Loading vocabulary: " << vocFile << " ... ";

    BOW_Object::mpORBvocabulary=SPtr<ORBVocabulary>(new ORBVocabulary);
    BOW_Object::mpORBvocabulary->loadFast(vocFile);
    cout << endl;

    // videoref from video or file
    std::string videoRefPath=svar.GetString("VideoRef.DataPath",refVideoPath+".ref");
    videoRef=SPtr<VideoRef>(new VideoRef(videoRefPath));
    if(!videoRef->loadFromFile(videoRefPath))
    {
        pi::timer.enter("TrainTotal");
        trainVideoRef(*videoRef,refVideoPath);
        pi::timer.leave("TrainTotal");
    }
    else
    {
        cout<<"VideoRef loaded from "<<videoRefPath<<"."<<endl;
    }

    // compare video difference
    findVideoDiff(*videoRef,svar.GetString("Video2Compare",""));
}

bool System::trainVideoRef(VideoRef& refVideo,const std::string& refVideoPath)
{
    cv::VideoCapture video(refVideoPath);
    if(!video.isOpened())
    {
        cerr<<"Can't open video "<<refVideoPath<<endl;
        return false;
    }
    VideoCompare compare(refVideo);
    cout<<"Training VideoRef from video "<<refVideoPath<<".\n";

    pi::TicTac ticToc;
    ticToc.Tic();

    double scale=svar.GetDouble("ComputeScale",0.5);
    int    trainSkip=svar.GetInt("TrainSkip",5);
    while(!shouldStop())
    {
        SPtr<VideoFrame> frame(new VideoFrame);
        for(int i=0;i<trainSkip;i++)
            video>>frame->img;
        video>>frame->img;

        if(frame->img.empty())
        {
            break;
        }

        if(scale<1)
        {
            cv::resize(frame->img,frame->img,cv::Size(frame->img.cols*scale,frame->img.rows*scale));
        }

        pi::timer.enter("TrainFrame");
        compare.train(frame);
        pi::timer.leave("TrainFrame");

        if(!svar.GetInt("WithQt"))
        {
            if(!compare.warpImg.empty())
                cv::imshow("warped",compare.warpImg);
            if(!compare.diffImg.empty())
                cv::imshow("diffImg",compare.diffImg);
            uchar key=cv::waitKey(20);
            if(key==27)   stop();
        }
    }

    return true;
}

bool System::findVideoDiff(VideoRef& refVideo,const std::string& videoPath)
{
    if(videoPath.empty())
    {
        cerr<<"Video2Compare not setted. Skip comparison. \n";
        return false;
    }

    cv::VideoCapture video(videoPath);
    if(!video.isOpened())
    {
        cerr<<"Can't open video "<<videoPath<<endl;
        return false;
    }
    VideoCompare compare(refVideo);
    cout<<"Find difference at video "<<videoPath<<".\n";

    pi::TicTac ticToc;
    ticToc.Tic();

    double scale=svar.GetDouble("ComputeScale",0.5);
    int    computeSkip=svar.GetInt("ComputeSkip",5);
    while(!shouldStop())
    {
        SPtr<VideoFrame> frame(new VideoFrame);
        for(int i=0;i<computeSkip;i++)
            video>>frame->img;
        video>>frame->img;

        if(frame->img.empty())
        {
            break;
        }

        if(scale<1)
        {
            cv::resize(frame->img,frame->img,
                       cv::Size(frame->img.cols*scale,frame->img.rows*scale));
        }

        pi::timer.enter("TrackDiff");
        compare.track(frame);
        pi::timer.leave("TrackDiff");

        if(!svar.GetInt("WithQt"))
        {
            if(!compare.warpImg.empty())
                cv::imshow("warped",compare.warpImg);
            if(!compare.diffImg.empty())
                cv::imshow("diffImg",compare.diffImg);
            uchar key=cv::waitKey(20);
            if(key==27)   stop();
        }
    }

    return true;
}
