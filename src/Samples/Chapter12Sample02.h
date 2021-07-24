#ifndef _H_CHAPTER12SAMPLE02_
#define _H_CHAPTER12SAMPLE02_

#include "Application.h"
#include "SamplePoseInstance.h"
#include "RHI/Rendering/DebugDraw.h"
#include "RHI/Rendering/Mesh.h"
#include "RHI/Texture.h"
#include <vector>
#include "Animation/Pose.h"
#include "Animation/Clip.h"
#include "Animation/Skeleton.h"
#include "Animation/SkinningType.h"
#include "AnimationBlending/Blending.h"
#include "AnimationBlending/Crossfading.h"

class Chapter12Sample02 : public Application {
protected:
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	std::vector<Mesh> mMeshes;
	std::vector<mat4> mPosePalette;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;

	DebugDraw* mBindPoseVisual;
	DebugDraw* mRestPoseVisual;
	DebugDraw* mCurrentPoseVisual;

	CrossFadeController mCrossfadingController;
	unsigned int mNextAnim;
	float mFadeTime;

	char** mUIClipNames;
	char** mSkinningNames;
	unsigned int mNumUIClips;
	bool mShowRestPose;
	bool mShowCurrentPose;
	bool mShowBindPose;
	bool mShowMesh;
	SkinningType mSkinType;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
	void ImGui(nk_context* inContext);
};

#endif