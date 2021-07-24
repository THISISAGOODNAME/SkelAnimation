#ifndef _H_CROSSFADING_
#define _H_CROSSFADING_

#include "Animation/Pose.h"
#include "Animation/Clip.h"
#include "Animation/Skeleton.h"
#include <vector>

struct CrossFadeTarget {
    Pose mPose;
    Clip* mClip;
    float mTime;
    float mDuration;
    float mElapsed;

    inline CrossFadeTarget() : mClip(nullptr), mTime(0.0f), mDuration(0.0f), mElapsed(0.0f) { }
    inline CrossFadeTarget(Clip* target, Pose& pose, float duration) : mClip(target), mTime(target->GetStartTime()), mPose(pose), mDuration(duration), mElapsed(0.0f) { }
};

class CrossFadeController {
protected:
    std::vector<CrossFadeTarget> mTargets;
    Clip* mClip;
    float mTime;
    Pose mPose;
    Skeleton mSkeleton;
    bool mWasSkeletonSet;

public:
    CrossFadeController();
    CrossFadeController(Skeleton& skeleton);
    void SetSkeleton(Skeleton& skeleton);
    void Play(Clip* target);
    void FadeTo(Clip* target, float fadeTime);
    void Update(float dt);
    Pose& GetCurrentPose();
    Clip* GetcurrentClip();
};

#endif //_H_CROSSFADING_
