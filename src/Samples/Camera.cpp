#include "Camera.h"
#include "MouseState.h"
#include "Math/quat.h"
#include <cmath>
#include <iostream>
#include "GLFW/glfw3.h"

inline float len(const vec2& v) {
    float lenSq = v.x * v.x + v.y * v.y;
    if (lenSq < VEC3_EPSILON) {
        return 0.0f;
    }
    return sqrtf(lenSq);
}

vec4 operator+(const vec4& l, const vec4& r)
{
    return vec4(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);
}

vec4 operator+(const vec4& l, const vec3& r)
{
    return vec4(l.x + r.x, l.y + r.y, l.z + r.z, l.w);
}

vec4 operator-(const vec4& l, const vec4& r)
{
    return vec4(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w);
}

vec4 operator-(const vec4& l, const vec3& r)
{
    return vec4(l.x - r.x, l.y - r.y, l.z - r.z, l.w);
}

vec2 operator*(const vec2& l, const float r)
{
    return vec2(l.x * r, l.y * r);
}


vec3 vec4To3(const vec4& v)
{
    return vec3(v.x, v.y, v.z);
}

void Camera::RotateAroundTargetWorld(const vec3& target, const vec2& delta)
{
    mat4 V = lookAt(mEye, mTarget, mUp);
    const vec3& pivot = target;
    const vec3 axis(-delta.y, -delta.x, 0);
    const float angle = len(delta) * QUAT_RAD2DEG;

    mat4 R = quatToMat4(angleAxis(angle, axis));

    mat4 T1, T2;
    T1.position = T1.position + pivot;
    T2.position = T2.position - pivot;

    mat4 NV = V * T1 * R * T2;
    mat4 C = inverse(NV);

    float target_dist = len(mTarget - mEye);
    mEye = vec3(C.position.x, C.position.y, C.position.z);
    mTarget = mEye - vec3(C.forward.x, C.forward.y, C.forward.z) * target_dist;
    mUp = vec3(C.up.x, C.up.y, C.up.z);
}

void Camera::RotateAroundTargetView(const vec3& target, const vec2& delta)
{
    mat4 V = lookAt(mEye, mTarget, mUp);

    vec4 pivot(V * vec4(target.x, target.y, target.z, 1.0f));
    pivot.w = 0.0f;
    const vec3 axis(-delta.y, -delta.x, 0);
    const float angle = len(delta) * QUAT_RAD2DEG;

    mat4 R = quatToMat4(angleAxis(angle, axis));

    mat4 T1, T2;
    T1.position = T1.position + pivot;
    T2.position = T2.position - pivot;

    mat4 NV = T1 * R * T2 * V;
    mat4 C = inverse(NV);

    float target_dist = len(mTarget - mEye);
    mEye = vec3(C.position.x, C.position.y, C.position.z);
    mTarget = mEye - vec3(C.forward.x, C.forward.y, C.forward.z) * target_dist;
    mUp = vec3(C.up.x, C.up.y, C.up.z);
}

void Camera::RotateAroundTarget(const vec2& delta)
{
    if (fabs(delta.x) > 0)
    {
        RotateAroundTargetWorld(mRealTarget, vec2(delta.x, 0.0));
    }

    if (fabs(delta.y) > 0)
    {
        RotateAroundTargetView(mRealTarget, vec2(0.0, delta.y));
    }
}

void Camera::Pan(const vec2& delta)
{
    vec3 d = cross(mTarget - mEye, mUp) * delta.x - mUp * delta.y;
    mEye = mEye + d;
    mTarget = mTarget + d;
}

void Camera::Zoom(const float delta)
{
    vec3 d = (mTarget - mEye) * delta;
    mEye = mEye + d; 
    mTarget = mTarget + d;
}

void Camera::update(const MouseState& inMouseState)
{
    if (inMouseState.Button == MouseButton::BUTTON_LEFT)
    {
        RotateAroundTarget(inMouseState.DeltaPos * 0.0003);
    }
    else if (inMouseState.Button == MouseButton::BUTTON_MIDDLE)
    {
        const float pan_speed = (0.1 + len(mEye - mRealTarget) * 0.001) * 0.05;
        Pan(inMouseState.DeltaPos * pan_speed);
    }
    else if (inMouseState.Button == MouseButton::BUTTON_RIGHT)
    {
        const float zoom_speed = fmax(0.01, len(mEye - mRealTarget) * 0.001);
        Zoom((-inMouseState.DeltaPos.x + inMouseState.DeltaPos.y) * zoom_speed);
    }
}

mat4 Camera::Perspective(const float inAspectRatio)
{
    return perspective(mFov, inAspectRatio, mZNear, mZFar);
}

mat4 Camera::View()
{
    return lookAt(mEye, mTarget, mUp);
}

Camera::Camera(const vec3& eye, const vec3& target, const vec3& up):
	mEye{ eye },
	mTarget{ target },
	mUp{ up },
    mRealTarget{ target },
    mZNear{ 0.01f },
    mZFar{ 1000.0f },
    mFov{ 60.0f }
{
}
