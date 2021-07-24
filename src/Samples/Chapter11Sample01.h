#ifndef _H_CHAPTER11SAMPLE01_
#define _H_CHAPTER11SAMPLE01_

#include "Samples/Framework/Application.h"
#include "RHI/Rendering/DebugDraw.h"
#include "RHI/Rendering/Mesh.h"
#include "RHI/Texture.h"
#include <vector>
#include "Animation/Pose.h"
#include "Animation/Clip.h"
#include "Animation/Skeleton.h"
#include "Animation/SkinningType.h"

class Chapter11Sample01 : public Application {
protected:
    Texture* mDiffuseTexture;
    Shader* mStaticShader;
    Shader* mSkinnedShader;
    std::vector<Mesh> mMeshes;
    std::vector<mat4> mPosePalette;
    Skeleton mSkeleton;
    Pose mCurrentPose;
    std::vector<Clip> mClips;
    unsigned int mCurrentClip;
    float mPlaybackTime;
    DebugDraw* mRestPoseVisual;
    DebugDraw* mCurrentPoseVisual;

    char** mUIClipNames;
    char** mSkinningNames;
    unsigned int mNumUIClips;
    bool mShowRestPose;
    bool mShowCurrentPose;
    SkinningType mSkinType;
public:
    void Initialize();
    void Update(float deltaTime);
    void Render(float inAspectRatio);
    void Shutdown();
    void ImGui(nk_context* inContext);
};

#endif