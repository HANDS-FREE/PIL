#ifndef BOW_OBJECT_H
#define BOW_OBJECT_H

#define ORB_BOW
#ifdef ORB_BOW
#include <opencv2/opencv.hpp>

#include "DBoW2/BowVector.h"
#include "DBoW2/FeatureVector.h"
#include "DBoW2/FORB.h"
#include "DBoW2/TemplatedVocabulary.h"

#include <base/Types/SPtr.h>
#include <base/Thread/Thread.h>

typedef DBoW2::FORB::TDescriptor ORB_Descriptor;
typedef DBoW2::TemplatedVocabulary<ORB_Descriptor, DBoW2::FORB>
  ORBVocabulary;

class BOW_Object
{
public:
    BOW_Object(){}
    BOW_Object(BOW_Object& obj)
//        :mBowVec(obj.GetBowVector()),mFeatVec(obj.GetFeatureVector())
    {
        mBowVec=obj.GetBowVector();
        mFeatVec=obj.GetFeatureVector();
    }

    virtual ~BOW_Object()
    {
//        mBowVec.~BowVector();
//        mFeatVec.~FeatureVector();
    }

    static std::vector<ORB_Descriptor> toDescriptorVector(const cv::Mat &Descriptors)
    {
        std::vector<ORB_Descriptor> vDesc;
        vDesc.reserve(Descriptors.rows);
        for (int j=0;j<Descriptors.rows;j++) {
            vDesc.push_back(*(DBoW2::FORB::TDescriptor*)Descriptors.row(j).data);
        }

        return vDesc;
    }

    // Bag of Words Representation
    void ComputeBoW(const cv::Mat &mDes_)
    {
        if(mBowVec.empty()||mFeatVec.empty())
        {
            vector<ORB_Descriptor> vCurrentDesc = toDescriptorVector(mDes_);

            // Feature vector associate features with nodes in the 4th level (from leaves up)
            // We assume the vocabulary tree has 6 levels, change the 4 otherwise
            mpORBvocabulary->transform(vCurrentDesc,mBowVec,mFeatVec,4);
        }
    }

    DBoW2::FeatureVector GetFeatureVector()
    {
        pi::ScopedMutex lock(mMutexBow);
        return mFeatVec;
    }

    DBoW2::BowVector GetBowVector()
    {
        pi::ScopedMutex lock(mMutexBow);
        return mBowVec;
    }

    //BoW
    DBoW2::BowVector     mBowVec;
    DBoW2::FeatureVector mFeatVec;
    pi::Mutex            mMutexBow;

    static SPtr<ORBVocabulary> mpORBvocabulary;
};
#endif

#endif // BOW_OBJECT_H
