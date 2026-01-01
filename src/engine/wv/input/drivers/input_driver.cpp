#include "input_driver.h"

#include <wv/input/input_system.h>

void wv::IInputDriver::handleTriggerAction( InputSystem* _inputSystem, TriggerAction* _action, bool _state )
{
	if ( _action->state == _state )
		return;

	_action->state = _state;
	_inputSystem->pushActionEvent( _action );
}

void wv::IInputDriver::handleValueAction( InputSystem* _inputSystem, ValueAction* _action, float _value )
{
	if ( _action->value == _value )
		return;

	_action->value = _value;
	_inputSystem->pushActionEvent( _action );
}

void wv::IInputDriver::handleAxisAction( InputSystem* _inputSystem, AxisAction* _action, AxisActionDirection _direction, const wv::Vector2f& _value, bool _additive )
{
	wv::Vector2f oldValue = _action->value;

	if ( _additive )
	{
		switch ( _direction )
		{
		case AXIS_DIRECTION_NORTH: _action->value.y += _value.x; break;
		case AXIS_DIRECTION_SOUTH: _action->value.y -= _value.x; break;
		case AXIS_DIRECTION_EAST:  _action->value.x += _value.x; break;
		case AXIS_DIRECTION_WEST:  _action->value.x -= _value.x; break;
		}
	}
	else
	{
		switch ( _direction )
		{
		case AXIS_DIRECTION_NORTH:      _action->value.y = _value.x; break;
		case AXIS_DIRECTION_SOUTH:      _action->value.y = _value.x; break;
		case AXIS_DIRECTION_EAST:       _action->value.x = _value.x; break;
		case AXIS_DIRECTION_WEST:       _action->value.x = _value.x; break;
		case AXIS_DIRECTION_VERTICAL:   _action->value = _value; break;
		case AXIS_DIRECTION_HORIZONTAL: _action->value = _value; break;
		case AXIS_DIRECTION_ALL:        _action->value = _value; break;
		}
	}

	if ( _action->value != oldValue )
		_inputSystem->pushActionEvent( _action );
}
