#ifndef _H_SKELETON_
#define _H_SKELETON_

#include "Animation/Pose.h"
#include "Math/mat4.h"
#include "Math/DualQuaternion.h"
#include <vector>
#include <string>

class Skeleton {
protected:
    Pose mRestPose;
    Pose mBindPose;
    std::vector<mat4> mInvBindPose;
    std::vector<std::string> mJointNames;

protected:
    void UpdateInverseBindPose();

public:
    Skeleton();
    Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);

    void Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);

    Pose& GetBindPose();
    Pose& GetRestPose();
    std::vector<mat4>& GetInvBindPose();

    std::vector<std::string>& GetJointNames();
    std::string& GetJointName(unsigned int index);

    void GetInvBindPose(std::vector<DualQuaternion>& outInvBndPose);
};

#endif
