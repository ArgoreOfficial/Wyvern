#include "controller_driver.h"

#include <wv/input/input_system.h>

void wv::IControllerDriver::handleTriggerAction( InputSystem* _inputSystem, TriggerAction* _action, bool _state )
{
	if ( _action->currentState == _state )
		return;

	_action->currentState = _state;
	_inputSystem->pushActionEvent( _action );
}

void wv::IControllerDriver::sendTriggerEvents( InputSystem* _inputSystem, ControllerDevice* _device )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getTriggerActionsByDevice( "Controller" ) )
		{
			switch ( mapping.inputID )
			{
			case CONTROLLER_BUTTON_A: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_A ) ); break;
			case CONTROLLER_BUTTON_B: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_B ) ); break;
			case CONTROLLER_BUTTON_X: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_X ) ); break;
			case CONTROLLER_BUTTON_Y: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_Y ) ); break;

			case CONTROLLER_BUTTON_DPAD_UP:    handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_DPAD_UP ) ); break;
			case CONTROLLER_BUTTON_DPAD_RIGHT: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_DPAD_RIGHT ) ); break;
			case CONTROLLER_BUTTON_DPAD_DOWN:  handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_DPAD_DOWN ) ); break;
			case CONTROLLER_BUTTON_DPAD_LEFT:  handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_DPAD_LEFT ) ); break;

			case CONTROLLER_BUTTON_START:  handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_START ) ); break;
			case CONTROLLER_BUTTON_SELECT: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_SELECT ) ); break;
			case CONTROLLER_BUTTON_HOME:   handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_HOME ) ); break;

			case CONTROLLER_BUTTON_JOYSTICK_LEFT:  handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_JOYSTICK_LEFT ) ); break;
			case CONTROLLER_BUTTON_JOYSTICK_RIGHT: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_JOYSTICK_RIGHT ) ); break;

			case CONTROLLER_BUTTON_SHOULDER_LEFT:  handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_SHOULDER_LEFT ) ); break;
			case CONTROLLER_BUTTON_SHOULDER_RIGHT: handleTriggerAction( _inputSystem, mapping.action, _device->getButtonState( CONTROLLER_BUTTON_SHOULDER_RIGHT ) ); break;
			}
		}
	}
}
