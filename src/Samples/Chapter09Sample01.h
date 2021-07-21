#ifndef _H_CHAPTER09SAMPLE01_
#define _H_CHAPTER09SAMPLE01_

#include "Application.h"
#include "RHI/Rendering/DebugDraw.h"
#include <vector>
#include "Animation/Pose.h"
#include "Animation/Clip.h"

class Chapter09Sample01 : public Application {
protected:
    Pose mRestPose;
    Pose mCurrentPose;
    std::vector<Clip> mClips;
    int mCurrentClip;
    float mPlaybackTime;
    DebugDraw* mRestPoseVisual;
    DebugDraw* mCurrentPoseVisual;

    char** mUIClipNames;
    unsigned int mNumUIClips;
    bool mShowRestPose;
    bool mShowCurrentPose;
public:
    void Initialize();
    void Update(float deltaTime);
    void Render(float inAspectRatio);
    void Shutdown();
    void ImGui(nk_context* inContext);
};

#endif
