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

	Ty getValue( int _playerIndex = -1 ) const {
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
	ActionType_Trigger,
	ActionType_Value,
	ActionType_Axis,

	ActionType_DeviceConnected,
	ActionType_DeviceDisconnected
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
	AxisActionDirection_North,
	AxisActionDirection_South,
	AxisActionDirection_East,
	AxisActionDirection_West,

	AxisActionDirection_Vertical,
	AxisActionDirection_Horizontal,

	AxisActionDirection_All // Used with joysticks
};

struct AxisActionMapping
{
	uint32_t inputID = 0;
	AxisActionDirection direction = AxisActionDirection_All;
	AxisAction* action = nullptr;
};

}