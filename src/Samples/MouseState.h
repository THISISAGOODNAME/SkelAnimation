#ifndef _H_MOUSESTATE_
#define _H_MOUSESTATE_

#include "Math/vec2.h"

enum MouseButton {
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_MIDDLE,
	BUTTON_NONE
};

struct MouseState {
	vec2 DeltaPos;
	vec2 Pos;
	MouseButton Button;
	float WheelDelta;

	inline void SetPos(const vec2& inPos) noexcept { DeltaPos = vec2(Pos.x - inPos.x, Pos.y - inPos.y); Pos = inPos; }
};
#endif