#pragma once

#include <wv/math/vector2.h>
#include <wv/debug/error.h>
#include <unordered_map>

namespace wv {

typedef uint32_t ActionID;

template<typename Ty>
struct IAction
{
	IAction( const std::string& _name ) : name{ _name } { }

	const std::string name;
	const ActionID actionID = wv::Math::randomU32();
	std::unordered_map<int, Ty> values = {}; 

	// player -1 is a special case but still needs to be tracked

	bool setValue( int _playerIndex, Ty _value ) {
		if ( _playerIndex < -1 ) return false;
		if ( values.contains( _playerIndex + 1 ) && values[ _playerIndex + 1 ] == _value ) return false;

		values[ _playerIndex + 1 ] = _value;
		return true;
	}

	Ty getValue( int _playerIndex ) const {
		if ( _playerIndex < -1 || !values.contains( _playerIndex + 1 ) ) return {};
		return values.at( _playerIndex + 1 );
	}
};

struct TriggerAction : IAction<bool> { 
	TriggerAction( const std::string& _name ) : IAction( _name ) { }
};

struct ValueAction : IAction<float> { 
	ValueAction( const std::string& _name ) : IAction( _name ) { }
};

struct AxisAction : IAction<wv::Vector2f> { 
	AxisAction( const std::string& _name ) : IAction( _name ) { }
};

enum ActionType
{
	ACTION_TYPE_TRIGGER,
	ACTION_TYPE_VALUE,
	ACTION_TYPE_AXIS,

	ACTION_DEVICE_CONNECTED,
	ACTION_DEVICE_DISCONNECTED
};

struct TriggerActionMapping
{
	uint32_t inputID;
	TriggerAction* action;
};

struct ValueActionMapping
{
	uint32_t inputID;
	ValueAction* action;
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