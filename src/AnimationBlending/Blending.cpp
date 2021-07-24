#include "Blending.h"
#include <iostream>

bool IsInHierarchy(Pose& pose, unsigned int parent, unsigned int search) {
    if (search == parent) {
        return true;
    }
    int p = pose.GetParent(search);

    while (p >= 0) {
        if (p == (int)parent) {
            return true;
        }
        p = pose.GetParent(p);
    }

    return false;
}

void Blend(Pose& output, Pose& a, Pose& b, float t, int blendroot) {
#if _DEBUG
    if (a.Size() != b.Size()) {
        std::cout << "Input poses have differing sizes\n";
    }
#endif

    unsigned int numJoints = output.Size();
    for (unsigned int i = 0; i < numJoints; ++i) {
        if (blendroot >= 0) {
            if (!IsInHierarchy(output, (unsigned int)blendroot, i)) {
                continue;
            }
        }

        output.SetLocalTransform(i, mix(a.GetLocalTransform(i), b.GetLocalTransform(i), t));
    }
}


