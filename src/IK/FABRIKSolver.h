#ifndef _H_FABRIKSOLVER_
#define _H_FABRIKSOLVER_

#include "Math/vec3.h"
#include "Math/Transform.h"
#include <vector>

// FABR - Forward And Backward Reaching
class FABRIKSolver {
protected:
    std::vector<Transform> mIKChain;
    std::vector<vec3> mWorldChain;
    std::vector<float> mLengths;
    unsigned int mNumSteps;
    float mThreshold;

protected:
    void IKChainToWorld();
    void WorldToIKChain();

    void IterateForward(const vec3& goal);
    void IterateBackward(const vec3& base);

public:
    FABRIKSolver();

    unsigned int Size();
    void Resize(unsigned int newSize);
    Transform GetLocalTransform(unsigned int index);
    void SetLocalTransform(unsigned int index, const Transform& t);
    Transform GetGlobalTransform(unsigned int index);

    unsigned int GetNumSteps();
    void SetNumSteps(unsigned int numSteps);

    float GetThreshold();
    void SetThreshold(float value);

    bool Solve(const Transform& target);
    bool Solve(const vec3& target);
};

#endif