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

void wv::IInputDriver::handleAxisAction( InputSystem* _inputSystem, uint32_t _vdID, AxisAction* _action, AxisActionDirection _direction, const wv::Vector2f& _value, bool _sendEvent, bool _additive )
{
	wv::Vector2f value = _action->getValue( _inputSystem->getDevicePlayer( _vdID ) );
	
	if ( _additive )
	{
		switch ( _direction )
		{
		case AxisActionDirection_North: value.y += _value.x; break;
		case AxisActionDirection_South: value.y -= _value.x; break;
		case AxisActionDirection_East:  value.x += _value.x; break;
		case AxisActionDirection_West:  value.x -= _value.x; break;
		}
	}
	else
	{
		switch ( _direction )
		{
		case AxisActionDirection_North:      value.y = _value.x; break;
		case AxisActionDirection_South:      value.y = _value.x; break;
		case AxisActionDirection_East:       value.x = _value.x; break;
		case AxisActionDirection_West:       value.x = _value.x; break;
		case AxisActionDirection_Vertical:   value = _value; break;
		case AxisActionDirection_Horizontal: value = _value; break;
		case AxisActionDirection_All:        value = _value; break;
		}
	}

	if ( !_action->setValue( _inputSystem->getDevicePlayer( _vdID ), value ) )
		return; // value was identical, return

	if( _sendEvent && _inputSystem->getActionsEnabled() )
		_inputSystem->pushActionEvent( _action, _vdID );
}
