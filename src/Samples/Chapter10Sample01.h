#ifndef _H_CHAPTER10SAMPLE01_
#define _H_CHAPTER10SAMPLE01_

#include "Application.h"
#include "RHI/Rendering/DebugDraw.h"
#include <vector>
#include "Animation/Skeleton.h"
#include "Animation/Clip.h"

class Chapter10Sample01 : public Application {
protected:
    Skeleton mSkeleton;
    Pose mCurrentPose;
    std::vector<Clip> mClips;
    unsigned int mCurrentClip;
    float mPlaybackTime;
    DebugDraw* mBindPoseVisual;
    DebugDraw* mRestPoseVisual;
    DebugDraw* mCurrentPoseVisual;

    char** mUIClipNames;
    unsigned int mNumUIClips;
    bool mShowRestPose;
    bool mShowCurrentPose;
    bool mShowBindPose;
public:
    void Initialize();
    void Update(float deltaTime);
    void Render(float inAspectRatio);
    void Shutdown();
    void ImGui(nk_context* inContext);
};

#endif