#include "input_driver.h"

#include <wv/input/input_system.h>

void wv::IInputDriver::handleTriggerAction( InputSystem* _inputSystem, uint32_t _vdID, TriggerAction* _action, bool _state )
{
	if ( !_action->setValue( _inputSystem->getDevicePlayer( _vdID ), _state ) )
		return;

	_inputSystem->pushActionEvent( _action, _vdID );
}

void wv::IInputDriver::handleValueAction( InputSystem* _inputSystem, uint32_t _vdID, ValueAction* _action, float _value )
{
	if ( !_action->setValue( _inputSystem->getDevicePlayer( _vdID ), _value ) )
		return;

	_inputSystem->pushActionEvent( _action, _vdID );
}

void wv::IInputDriver::handleAxisAction( InputSystem* _inputSystem, uint32_t _vdID, AxisAction* _action, AxisActionDirection _direction, const wv::Vector2f& _value, bool _additive )
{
	wv::Vector2f value = _action->getValue( _inputSystem->getDevicePlayer( _vdID ) );
	
	if ( _additive )
	{
		switch ( _direction )
		{
		case AXIS_DIRECTION_NORTH: value.y += _value.x; break;
		case AXIS_DIRECTION_SOUTH: value.y -= _value.x; break;
		case AXIS_DIRECTION_EAST:  value.x += _value.x; break;
		case AXIS_DIRECTION_WEST:  value.x -= _value.x; break;
		}
	}
	else
	{
		switch ( _direction )
		{
		case AXIS_DIRECTION_NORTH:      value.y = _value.x; break;
		case AXIS_DIRECTION_SOUTH:      value.y = _value.x; break;
		case AXIS_DIRECTION_EAST:       value.x = _value.x; break;
		case AXIS_DIRECTION_WEST:       value.x = _value.x; break;
		case AXIS_DIRECTION_VERTICAL:   value = _value; break;
		case AXIS_DIRECTION_HORIZONTAL: value = _value; break;
		case AXIS_DIRECTION_ALL:        value = _value; break;
		}
	}

	if ( _action->setValue( _inputSystem->getDevicePlayer( _vdID ), value ) )
		_inputSystem->pushActionEvent( _action, _vdID );
}
