#ifndef _H_CHAPTER15SAMPLE01_
#define _H_CHAPTER15SAMPLE01_

#include "Samples/Framework/Application.h"
#include "RHI/Rendering/DebugDraw.h"
#include "RHI/Rendering/Mesh.h"
#include "RHI/Texture.h"
#include <vector>
#include "Animation/Pose.h"
#include "Animation/Clip.h"
#include "Animation/Skeleton.h"
#include "Animation/SkinningType.h"
#include "Crowds/AnimTexture.h"
#include "Crowds/Crowd.h"

class Chapter15Sample01 : public Application {
protected:
	Texture* mDiffuseTexture;
	Shader* mCrowdShader;
	std::vector<Mesh> mMeshes;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;
	std::vector<AnimTexture> mTextures;
	std::vector<Crowd> mCrowds;
protected:
	void SetCrowdSize(unsigned int size);
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif