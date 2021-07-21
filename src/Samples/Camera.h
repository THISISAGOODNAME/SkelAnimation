#ifndef _H_CAMERA_
#define _H_CAMERA_

#include "Math/vec3.h"
#include "Math/vec2.h"
#include "Math/mat4.h"

struct MouseState;
class Camera {
private:
	vec3 mEye;
	vec3 mTarget;
	vec3 mUp;
	vec3 mRealTarget;
	float mZNear;
	float mZFar;
	float mFov;

	void RotateAroundTargetWorld(const vec3& target, const vec2& delta);
	void RotateAroundTargetView(const vec3& target, const vec2& delta);
	void RotateAroundTarget(const vec2& delta);
	void Pan(const vec2& delta);
	void Zoom(const float delta);
public:
	Camera(const vec3& eye = vec3(0, 0, -2), const vec3& target = vec3(0, 0, 0), const vec3& up = vec3(0, 1, 0));

	inline vec3 GetEye() const noexcept { return mEye; }
	inline vec3 GetTarget() const noexcept { return mRealTarget; }
	inline vec3 GetUp() const noexcept { return mUp; }

	inline void setTarget(const vec3& inTarget) noexcept { mRealTarget = inTarget; }
	inline void setZNear(const float inZNear) noexcept { mZNear = inZNear; }
	inline void setZFar(const float inZFar) noexcept { mZFar = inZFar; }
	inline void setFov(const float inFov) noexcept { mFov = inFov; }

	void update(const MouseState& inMouseState);
	mat4 View();
	mat4 Perspective(const float inAspectRatio);
};
#endif