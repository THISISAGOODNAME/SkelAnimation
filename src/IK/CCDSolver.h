#ifndef _H_CCDSOLVER_
#define _H_CCDSOLVER_

#include "Math/Transform.h"
#include <vector>

// CCD - Cyclic Coordinate Descent
class CCDSolver {
protected:
    std::vector<Transform> mIKChain;
    unsigned int mNumSteps;
    float mThreshold;

public:
    CCDSolver();

    unsigned int Size();
    void Resize(unsigned int newSize);

    Transform& operator[](unsigned int index);
    Transform GetGlobalTransform(unsigned int index);

    Transform GetLocalTransform(unsigned int index);
    void SetLocalTransform(unsigned int index, const Transform& t);

    unsigned int GetNumSteps();
    void SetNumSteps(unsigned int numSteps);

    float GetThreshold();
    void SetThreshold(float value);

    bool Solve(const Transform& target);
    bool Solve(const vec3& target);
};

#endif //_H_CCDSOLVER_
