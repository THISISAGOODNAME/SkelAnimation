#ifndef _H_MESH_
#define _H_MESH_

#include "Math/vec2.h"
#include "Math/vec3.h"
#include "Math/vec4.h"
#include "Math/mat4.h"
#include "RHI/Attribute.h"
#include "RHI/IndexBuffer.h"
#include "Animation/Skeleton.h"
#include <vector>

class Mesh {
protected:
    std::vector<vec3> mPosition;
    std::vector<vec3> mNormal;
    std::vector<vec2> mTexCoord;
    std::vector<vec4> mWeights;
    std::vector<ivec4> mInfluences;
    std::vector<unsigned int> mIndices;

protected:
    Attribute<vec3>* mPosAttrib;
    Attribute<vec3>* mNormAttrib;
    Attribute<vec2>* mUvAttrib;
    Attribute<vec4>* mWeightAttrib;
    Attribute<ivec4>* mInfluenceAttrib;
    IndexBuffer* mIndexBuffer;

protected:
    std::vector<vec3> mSkinnedPosition;
    std::vector<vec3> mSkinnedNormal;
    std::vector<mat4> mPosePalette;

public:
    Mesh();
    Mesh(const Mesh&);
    Mesh& operator=(const Mesh&);
    ~Mesh();

    std::vector<vec3>& GetPosition();
    std::vector<vec3>& GetNormal();
    std::vector<vec2>& GetTexCoord();
    std::vector<vec4>& GetWeights();
    std::vector<ivec4>& GetInfluences();
    std::vector<unsigned int>& GetIndices();

    void CPUSkin(Skeleton& skeleton, Pose& pose);
    void CPUSkin(std::vector<mat4>& animated);
    void UpdateOpenGLBuffers();

    void Bind(int position, int normal, int texCoord, int weight, int influcence);
    void UnBind(int position, int normal, int texCoord, int weight, int influcence);
    void Draw();
    void DrawInstanced(unsigned int numInstances);
};

#endif
