#pragma once

#include <wv/event/event.h>
#include <wv/math/vector2.h>

#include <wv/input/input_events.h>

namespace wv {

class MouseMoveEvent : public IEvent
{
	WV_REFLECT_TYPE( MouseMoveEvent )
public:
	Vector2f move{};
	Vector2i position{};
};

class MouseButtonEvent : public IEvent
{
	WV_REFLECT_TYPE( MouseButtonEvent )
public:
	inline bool isDown() const { return state; }
	inline bool isUp()   const { return !state; }

	bool state = false;
	int buttonID = 0;
};

class KeyboardEvent : public IEvent
{
	WV_REFLECT_TYPE( KeyboardEvent )
public:
	inline bool isDown() const { return state; }
	inline bool isUp()   const { return !state; }

	bool state = false;
	bool isRepeat = false;
	Scancode scancode = wv::SCANCODE_NONE;
};

class ControllerButtonEvent : public IEvent
{
	WV_REFLECT_TYPE( ControllerButtonEvent )
public:
	inline bool isDown() const { return state; }
	inline bool isUp()   const { return !state; }

	bool state = false;
	ControllerButton button = wv::CONTROLLER_BUTTON_NONE;
};

}