#include "controller_driver.h"

#include <wv/input/input_system.h>
#include <inttypes.h>

void wv::IControllerDriver::sendTriggerEvents( InputSystem* _inputSystem, ControllerDevice* _device )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		for ( auto& mapping : group->getTriggerActionsByDevice( "Controller" ) )
		{
			if( mapping.inputID >= ControllerInput_ButtonA && mapping.inputID <= ControllerInput_ShoulderRight )
				handleTriggerAction( _inputSystem, _device->vdID, mapping.action, _device->getButtonState( (ControllerInputs)mapping.inputID ), group->isEnabled() );
			else
				WV_LOG_WARNING( "Input ID %u is not handled as a TriggerAction input.\n", mapping.inputID );
		}
	}
}

void wv::IControllerDriver::sendValueEvents( InputSystem* _inputSystem, ControllerDevice* _device, ControllerDevice* _prevDeviceState )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		for ( auto& mapping : group->getValueActionsByDevice( "Controller" ) )
		{
			const ControllerInputs inputID = (ControllerInputs)mapping.inputID;

			switch ( inputID )
			{
			case ControllerInput_TriggerLeft:
				if ( _device->leftTrigger != _prevDeviceState->leftTrigger )
					handleValueAction( _inputSystem, _device->vdID, mapping.action, _device->leftTrigger, group->isEnabled() );
				break;

			case ControllerInput_TriggerRight:
				if ( _device->rightTrigger != _prevDeviceState->rightTrigger )
					handleValueAction( _inputSystem, _device->vdID, mapping.action, _device->rightTrigger, group->isEnabled() );
				break;

			default:
			{
				// if mapping is button
				if ( inputID >= ControllerInput_ButtonA && inputID <= ControllerInput_ShoulderRight )
				{
					if ( _device->getButtonState( inputID ) != _prevDeviceState->getButtonState( inputID ) )
						handleValueAction( _inputSystem, _device->vdID, mapping.action, _device->getButtonState( inputID ) ? 1.0f : 0.0f, group->isEnabled() );
				}
				else
					WV_LOG_WARNING( "Input ID %u is not handled as a ValueAction input.\n", inputID );
			}
			}
		}
	}
}

void wv::IControllerDriver::sendAxisEvents( InputSystem* _inputSystem, ControllerDevice* _device, ControllerDevice* _prevDeviceState )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		for ( auto& mapping : group->getAxisActionsByDevice( "Controller" ) )
		{
			switch ( mapping.inputID )
			{
			case ControllerInput_DpadUp:    [[fallthrough]];
			case ControllerInput_DpadRight: [[fallthrough]];
			case ControllerInput_DpadDown:  [[fallthrough]];
			case ControllerInput_DpadLeft:
				if ( _device->getButtonState( (ControllerInputs)mapping.inputID ) != _prevDeviceState->getButtonState( (ControllerInputs)mapping.inputID ) )
				{
					handleAxisAction( 
						_inputSystem, 
						_device->vdID,
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
				
			case ControllerInput_JoystickLeft:  
				if( _device->leftJoystick != _prevDeviceState->leftJoystick )
					handleAxisAction( _inputSystem, _device->vdID, mapping.action, mapping.direction, _device->leftJoystick, group->isEnabled() );
				break;

			case ControllerInput_JoystickRight: 
				if( _device->rightJoystick != _prevDeviceState->rightJoystick )
					handleAxisAction( _inputSystem, _device->vdID, mapping.action, mapping.direction, _device->rightJoystick, group->isEnabled() );
				break;

			default:
				WV_LOG_WARNING( "Input ID %u is not handled as an AxisAction input.\n", mapping.inputID );
			}
		}
	}
}
