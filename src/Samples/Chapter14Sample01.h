#ifndef _H_CHAPTER14SAMPLE01_
#define _H_CHAPTER14SAMPLE01_

#include "Samples/Framework/Application.h"
#include "RHI/Rendering/DebugDraw.h"
#include "RHI/Rendering/Mesh.h"
#include "RHI/Texture.h"
#include <vector>
#include "Animation/Pose.h"
#include "Animation/Clip.h"
#include "Animation/Skeleton.h"
#include "Animation/SkinningType.h"

class Chapter14Sample01 : public Application {
protected:
	Texture* mDiffuseTexture;
	Shader* mLBSShader;
	Shader* mDQShader;
	std::vector<Mesh> mMeshes;
	Skeleton mSkeleton;
	Pose mCurrentPose;

	std::vector<DualQuaternion> mDqPosePalette;
	std::vector<DualQuaternion> mDqInvBindPalette;
	std::vector<mat4> mLbPosePalette;
	std::vector<mat4> mLbInvBindPalette;
	
	std::vector<Clip> mClips;
	unsigned int mCurrentClip;
	float mPlaybackTime;

	char** mSkinningNames;
	int mSkinningMethod;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
	void ImGui(nk_context* inContext);
};

#endif