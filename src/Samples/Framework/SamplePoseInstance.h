#ifndef _H_SAMPLE_POSE_INSTANCE_
#define _H_SAMPLE_POSE_INSTANCE_

#include "Animation/Pose.h"
#include "RHI/Rendering/DebugDraw.h"

struct SamplePoseInstance {
    Pose mPose;
    unsigned int mClip;
    float mPlayback;
    DebugDraw* mVisual;
    bool mShowPose;

    inline SamplePoseInstance() : mClip(0), mPlayback(0.0f), mVisual(0), mShowPose(false){ }
};

#endif //_H_SAMPLE_POSE_INSTANCE_
