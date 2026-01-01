#pragma once

#include <wv/math/vector2.h>

namespace wv {

typedef uint32_t ActionID;

enum ActionType
{
	ACTION_TYPE_TRIGGER,
	ACTION_TYPE_VALUE,
	ACTION_TYPE_AXIS
};

struct TriggerAction
{
	TriggerAction( const std::string& _name ) : name{ _name } { }

	const std::string name;
	const ActionID actionID = wv::Math::randomU32();
	bool state = false;
};

struct TriggerActionMapping
{
	uint32_t inputID;
	TriggerAction* action;
};

struct ValueAction
{
	ValueAction( const std::string& _name ) : name{ _name } { }

	const std::string name;
	const ActionID actionID = wv::Math::randomU32();
	float value = 0.0f;
};

struct ValueActionMapping
{
	uint32_t inputID;
	ValueAction* action;
};

struct AxisAction
{
	AxisAction( const std::string& _name ) : name{ _name } { }

	const std::string name;
	const ActionID actionID = wv::Math::randomU32();

	wv::Vector2f value;
};

enum AxisActionDirection
{
	AXIS_DIRECTION_NORTH,
	AXIS_DIRECTION_SOUTH,
	AXIS_DIRECTION_EAST,
	AXIS_DIRECTION_WEST,

	AXIS_DIRECTION_VERTICAL,
	AXIS_DIRECTION_HORIZONTAL,

	AXIS_DIRECTION_ALL // Used with joysticks
};

struct AxisActionMapping
{
	uint32_t inputID = 0;
	AxisActionDirection direction = AXIS_DIRECTION_ALL;
	AxisAction* action = nullptr;
};

}