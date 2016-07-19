#include "VideoCompare.h"
#include <base/Svar/Svar.h>

#define TH_LOW 50

VideoCompare::VideoCompare(VideoRef& ref)
    :videoRef(ref),
      extractor(svar.GetInt("ORBextractor.nFeatures",1000)),
      refImg(SvarWithType<cv::Mat>::instance()["VideoCompare.RefImage"]),
      trackImg(SvarWithType<cv::Mat>::instance()["VideoCompare.TrackImage"]),
      warpImg(SvarWithType<cv::Mat>::instance()["VideoCompare.WarpImage"]),
      diffImg(SvarWithType<cv::Mat>::instance()["VideoCompare.DiffImage"])
{
}

// Bit set count operation from
// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
int VideoCompare::DescriptorDistance(const cv::Mat &a, const cv::Mat &b)
{
    const int *pa = a.ptr<int32_t>();
    const int *pb = b.ptr<int32_t>();

    int dist=0;

    for(int i=0; i<8; i++, pa++, pb++)
    {
        unsigned  int v = *pa ^ *pb;
        v = v - ((v >> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
        dist += (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
    }

    return dist;
}

bool VideoCompare::checkFrame(SPtr<VideoFrame>& frame)
{
    if(frame->img_gray.empty())
    {
        cv::cvtColor(frame->img,frame->img_gray,CV_BGR2GRAY);
    }
    frame->extractorFeatures(extractor);
    return true;
}

// nowX=initH*refX
bool VideoCompare::trackFrame(SPtr<VideoFrame>& frame,SPtr<VideoFrame>& ref,cv::Mat initH)
{
    std::vector<cv::KeyPoint>& keyPoints=frame->mKeyPoints;
    cv::Mat& descriptors=frame->mDescriptors;

    // findHomography & warpPerspective
    invert(initH,initH);
    double* this2KF=(double*)initH.data;

    vector<cv::Point2f> herePts, kfPts;

    for(size_t i=0;i<keyPoints.size();i++)
    {
        cv::KeyPoint& kpt=keyPoints[i];
        cv::Point2d   dpt=mult(this2KF,kpt.pt);

        vector<size_t> vIndices = ref->GetFeaturesInArea(dpt.x,dpt.y,20);

        if(vIndices.empty())
            continue;

        // Match to the most similar keypoint in the radius

        cv::Mat des=descriptors.row(i);
        int bestDist = INT_MAX;
        int bestIdx = -1;
        int minOctave=kpt.octave-1;
        int maxOctove=kpt.octave+1;
        for(vector<size_t>::iterator vit=vIndices.begin(), vend=vIndices.end(); vit!=vend; vit++)
        {
            const size_t idx = *vit;
            const int kpLevel= ref->GetKeyPointScaleLevel(idx);

            if(kpLevel<minOctave || kpLevel>maxOctove)
                continue;

            cv::Mat dKF = ref->mDescriptors.row(idx);

            const int dist = DescriptorDistance(des,dKF);

            if(dist<bestDist)
            {
                bestDist = dist;
                bestIdx = idx;
            }
        }

        // If there is already a MapPoint replace otherwise add new measurement
        if(bestDist<=TH_LOW)
        {
            herePts.push_back(kpt.pt);
            kfPts.push_back(ref->mKeyPoints[bestIdx].pt);
        }
    }

    if(herePts.size()<20) return false;

    vector<uchar>  inlierFlags;
    frame->H2Last=cv::findHomography(kfPts,herePts,inlierFlags, cv::RANSAC, 10);

    //debug things and publish
    {
        // warp kf to here
        refImg=ref->img;
        trackImg=frame->img;
        warpImg=trackImg.clone();
        cv::Mat H=frame->H2Last.clone();
        cv::warpPerspective(ref->img,warpImg,H,
                            cv::Size(warpImg.cols,warpImg.rows),cv::INTER_LINEAR,cv::BORDER_TRANSPARENT);

        cv::absdiff(trackImg,warpImg,diffImg);

        // draw keypoints
        for(size_t i=0;i<herePts.size();i++)
        {
            if(inlierFlags[i])
                cv::circle(warpImg,herePts[i],2,cv::Scalar(255,0,0),2);
            else
                cv::circle(warpImg,herePts[i],2,cv::Scalar(0,255,0),2);
        }
    }
    return true;
}

bool VideoCompare::train(SPtr<VideoFrame>& frame)
{
    if(!checkFrame(frame)) return false;
    // track last frame and ...
    if(!lastKeyframe.get())// the first or after losted
    {
        H2Last=cv::Mat::eye(3,3,CV_64F);
        lastKeyframe=frame;
        videoRef.addKeyframe(lastKeyframe);
        return true;
    }

    if(!trackFrame(frame,lastKeyframe,H2Last))// losted
    {
        lastKeyframe=SPtr<VideoFrame>();
        return false;
    }

    float shareP=sharedPercent(frame->img.cols,frame->img.rows,frame->H2Last);
    if(shareP<0.9)
    {
        lastKeyframe=frame;// new keyframe
        videoRef.addKeyframe(frame);
        H2Last=cv::Mat::eye(3,3,CV_64F);
    }
    else
    {
        H2Last=frame->H2Last;
    }
    return true;
}


bool VideoCompare::trackFrame(SPtr<VideoFrame>& frame,SPtr<VideoFrame>& ref)
{
    frame->ComputeBoW();

    DBoW2::FeatureVector frmVec = frame->GetFeatureVector();
    DBoW2::FeatureVector refVec = ref->GetFeatureVector();

    DBoW2::FeatureVector::iterator f1it = frmVec.begin();
    DBoW2::FeatureVector::iterator f2it = refVec.begin();
    DBoW2::FeatureVector::iterator f1end = frmVec.end();
    DBoW2::FeatureVector::iterator f2end = refVec.end();

    vector<cv::Point2f> herePts, kfPts;

    while(f1it!=f1end && f2it!=f2end)
    {
        if(f1it->first == f2it->first)
        {
            for(size_t i1=0, iend1=f1it->second.size(); i1<iend1; i1++)
            {
                size_t idx1 = f1it->second[i1];

                cv::Mat d1 = frame->mDescriptors.row(idx1);

                vector<pair<int,size_t> > vDistIndex;

                for(size_t i2=0, iend2=f2it->second.size(); i2<iend2; i2++)
                {
                    size_t idx2 = f2it->second[i2];

                    cv::Mat d2 = frame->mDescriptors.row(idx2);

                    const int dist = DescriptorDistance(d1,d2);

                    if(dist>TH_LOW)
                        continue;

                    vDistIndex.push_back(make_pair(dist,idx2));
                }

                if(vDistIndex.empty())
                    continue;

                sort(vDistIndex.begin(),vDistIndex.end());
                herePts.push_back(frame->mKeyPoints[idx1].pt);
                kfPts.push_back(ref->mKeyPoints[vDistIndex.front().second].pt);
            }

            f1it++;
            f2it++;
        }
        else if(f1it->first < f2it->first)
        {
            f1it = frmVec.lower_bound(f2it->first);
        }
        else
        {
            f2it = refVec.lower_bound(f1it->first);
        }
    }



    if(herePts.size()<20) return false;

    vector<uchar>  inlierFlags;
    frame->H2Last=cv::findHomography(kfPts,herePts,inlierFlags, cv::RANSAC, 10);

    //debug things and publish
    {
        // warp kf to here
        refImg=ref->img;
        trackImg=frame->img;
        warpImg=trackImg.clone();
        cv::Mat H=frame->H2Last.clone();
        cv::warpPerspective(ref->img,warpImg,H,
                            cv::Size(warpImg.cols,warpImg.rows),cv::INTER_LINEAR,cv::BORDER_TRANSPARENT);
        // draw keypoints
        for(size_t i=0;i<herePts.size();i++)
        {
            if(inlierFlags[i])
                cv::circle(warpImg,herePts[i],2,cv::Scalar(255,0,0),2);
            else
                cv::circle(warpImg,herePts[i],2,cv::Scalar(0,255,0),2);
        }
    }
    return true;
}

bool VideoCompare::track(SPtr<VideoFrame>& frame)
{
        if(!checkFrame(frame)) return false;
        // track last frame and ...
        if(!lastKeyframe.get())// relocalization
        {
            frame->ComputeBoW();
            map<FrameID,uint> candidates=videoRef.dbow.DetectRelocalisationCandidates(frame.get());
            if(!candidates.size()) return false;
            // find the max similarity
            uint maxSharedWord=1;
            for(map<FrameID,uint>::iterator it=candidates.begin();it!=candidates.end();it++)
            {
                if(it->second>maxSharedWord)
                maxSharedWord=it->second;
            }

            vector<std::pair<uint,FrameID>> candVec;
            candVec.reserve(candidates.size());
            for(map<FrameID,uint>::iterator it=candidates.begin();it!=candidates.end();it++)
            {
                if(it->second<maxSharedWord*0.7) continue;
                candVec.push_back(make_pair(it->second,it->first));
            }

            std::sort(candVec.begin(),candVec.end());

            for(int i=candVec.size()-1;i>=0;i--)
            {
                SPtr<VideoFrame> ref=videoRef.frames[candVec[i].second];
                if(trackFrame(frame,ref))
                {
                    lastKeyframe=ref;
                    H2Last=frame->H2Last;
                    return true;
                }
            }

            return false;
        }

        if(!trackFrame(frame,lastKeyframe,H2Last))// losted
        {
            lastKeyframe=SPtr<VideoFrame>();
            return false;
        }


        H2Last=frame->H2Last;
        // check if we need to change ref keyframe
        {
            SPtr<VideoFrame> prevFrame,nextFrame;
            if(lastKeyframe->id>0)
                prevFrame=videoRef.frames[lastKeyframe->id-1];
            if(lastKeyframe->id<videoRef.frames.size()-1)
                nextFrame=videoRef.frames[lastKeyframe->id+1];


            float prevShare=0,thisShare,nextShare=0;
            int w=frame->img.cols;
            int h=frame->img.rows;
            thisShare=sharedPercent(w,h,H2Last);
            if(nextFrame.get()&&!nextFrame->H2Last.empty())
            {
                cv::Mat H=H2Last*nextFrame->H2Last.inv();
                nextShare=sharedPercent(w,h,H);

                if(nextShare>thisShare*1.05)
                {
                    cout<<"New RefKF:"<<nextFrame->id
                       <<",ThisShare:"<<thisShare
                      <<",NextShare:"<<nextShare<<endl;
                    lastKeyframe=nextFrame;
                    H2Last=H;
                }

            }

            if(prevFrame.get()&&!lastKeyframe->H2Last.empty())
            {
                cv::Mat H=H2Last*lastKeyframe->H2Last;
                prevShare=sharedPercent(w,h,H);

                if(prevShare>thisShare*1.05)
                {
                    cout<<"New RefKF:"<<prevFrame->id
                       <<",ThisShare:"<<thisShare
                      <<",PrevShare:"<<prevShare<<endl;
                    lastKeyframe=prevFrame;
                    H2Last=H;
                }
            }

        }


        return true;
}

float VideoCompare::sharedPercent(int w,int h,cv::Mat H)
{
    return (w-fabs(H.at<double>(0,2)))*(h-fabs(H.at<double>(1,2)))/(double)(w*h);
}
