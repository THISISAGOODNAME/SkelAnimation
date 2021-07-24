#ifndef _H_CHAPTER06_SAMPLE01_
#define _H_CHAPTER06_SAMPLE01_

#include "Math/vec3.h"
#include "Math/vec2.h"

#include "Samples/Framework/Application.h"
#include "RHI/Shader.h"
#include "RHI/Attribute.h"
#include "RHI/IndexBuffer.h"
#include "RHI/Texture.h"

class Chapter06Sample01 : public Application {
protected:
	Shader* mShader;
	Attribute<vec3>* mVertexPositions;
	Attribute<vec3>* mVertexNormals;
	Attribute<vec2>* mVertexTexCoords;
	IndexBuffer* mIndexBuffer;
	Texture* mDisplayTexture;
	float mRotation;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif