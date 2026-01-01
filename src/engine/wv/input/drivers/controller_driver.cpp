#include "controller_driver.h"

#include <wv/input/input_system.h>
#include <inttypes.h>

void wv::IControllerDriver::handleTriggerAction( InputSystem* _inputSystem, TriggerAction* _action, bool _state )
{
	if ( _action->currentState == _state )
		return;

	_action->currentState = _state;
	_inputSystem->pushActionEvent( _action );
}

void wv::IControllerDriver::handleAxisAction( InputSystem* _inputSystem, AxisAction* _action, AxisActionDirection _direction, const wv::Vector2f& _value, bool _additive )
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
	{
		_inputSystem->pushActionEvent( _action );
		wv::Debug::Print( "%s: %f,%f\n", _action->name.c_str(), _action->value.x, _action->value.y);
	}
}

void wv::IControllerDriver::sendTriggerEvents( InputSystem* _inputSystem, ControllerDevice* _device )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getTriggerActionsByDevice( "Controller" ) )
		{
			if( mapping.inputID >= CONTROLLER_BUTTON_A && mapping.inputID <= CONTROLLER_BUTTON_SHOULDER_RIGHT )
				handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( (ControllerInputs)mapping.inputID ) ); 
			else
				WV_LOG_WARNING( "Input ID " PRIu32 " is not handled as a trigger input.\n", mapping.inputID );
		}
	}
}

void wv::IControllerDriver::sendAxisEvents( InputSystem* _inputSystem, ControllerDevice* _device, ControllerDevice* _prevDeviceState )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getAxisActionsByDevice( "Controller" ) )
		{
			switch ( mapping.inputID )
			{
			case CONTROLLER_BUTTON_DPAD_UP:    [[fallthrough]];
			case CONTROLLER_BUTTON_DPAD_RIGHT: [[fallthrough]];
			case CONTROLLER_BUTTON_DPAD_DOWN:  [[fallthrough]];
			case CONTROLLER_BUTTON_DPAD_LEFT:
				if ( _device->getButtonState( (ControllerInputs)mapping.inputID ) != _prevDeviceState->getButtonState( (ControllerInputs)mapping.inputID ) )
				{
					handleAxisAction( 
						_inputSystem, 
						mapping.action, 
						mapping.direction, 
						{ // -1.0 to 1.0 because buttons are additive, see handleAxisAction
							_device->getButtonState( (ControllerInputs)mapping.inputID ) ? 1.0f : -1.0f, 
							0.0f 
						},
						true
					); 
				}
				break;
				
			case CONTROLLER_JOYSTICK_LEFT:  
				if( _device->leftJoystick != _prevDeviceState->leftJoystick )
					handleAxisAction( _inputSystem, mapping.action, mapping.direction, _device->leftJoystick );  
				break;

			case CONTROLLER_JOYSTICK_RIGHT: 
				if( _device->rightJoystick != _prevDeviceState->rightJoystick )
					handleAxisAction( _inputSystem, mapping.action, mapping.direction, _device->rightJoystick ); 
				break;

			default:
				WV_LOG_WARNING( "Input ID " PRIu32 " is not handled as an axis input.\n", mapping.inputID );
			}
		}
	}
}
