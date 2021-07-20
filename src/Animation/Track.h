#ifndef _H_TRACK_
#define _H_TRACK_

#include "Frame.h"
#include "Interpolation.h"
#include "Math/vec3.h"
#include "Math/quat.h"

#include <vector>

template<typename T, int N>
class Track {

protected:
    std::vector<Frame<N>> mFrames;
    Interpolation mInterpolation;

public:
    Track();

    void Resize(unsigned int size);
    unsigned int Size();

    Interpolation GetInterpolation();
    void SetInterpolation(Interpolation interpolation);

    float GetStartTime();
    float GetEndTime();

    T Sample(float time, bool looping);
    Frame<N>& operator[](unsigned int index);

protected:
    T SampleConstant(float time, bool looping);
    T SampleLinear(float time, bool looping);
    T SampleCubic(float time, bool looping);

    T Hermite(float time, const T& point1, const T& slope1, const T& point2, const T& slope2); // Will have one specialization for Quat

    virtual int FrameIndex(float time, bool looping);
    float AdjustTimeToFitTrack(float time, bool looping);
    T Cast(float* value); // Will be specialized
};

typedef Track<float, 1> ScalarTrack;
typedef Track<vec3, 3> VectorTrack;
typedef Track<quat, 4> QuaternionTrack;

#endif
