#include "xinput_controller_driver.h"

#include <wv/input/input_system.h>

void wv::XInputControllerDriver::updateDriver( InputSystem* _inputSystem )
{
	XINPUT_STATE state{};

	for ( size_t i = 0; i < XUSER_MAX_COUNT; i++ )
	{
		if ( XInputGetState( i, &state ) == ERROR_SUCCESS )
		{
			if ( !m_connectedDeviceIDs.contains( i ) ) // device was just connected
				handleDeviceConnected( i );

			updateDeviceState( _inputSystem, i, state );
		}
		else
		{
			if ( m_connectedDeviceIDs.contains( i ) ) // device was just disconnected
				handleDeviceDisconnected( i );
		}
	}

}

void wv::XInputControllerDriver::handleDeviceConnected( int _deviceID )
{
	wv::Debug::Print( "XInput Device %i connected\n", _deviceID );
	m_connectedDeviceIDs.insert( _deviceID );

	ControllerDevice* device = WV_NEW( ControllerDevice );
	device->deviceID = _deviceID;

	m_connectedDevices.push_back( device );
	
}

void wv::XInputControllerDriver::handleDeviceDisconnected( int _deviceID )
{
	wv::Debug::Print( "XInput Device %i disconnected\n", _deviceID );

	m_connectedDeviceIDs.erase( _deviceID );

	auto it = getDevice( _deviceID );
	if ( it == m_connectedDevices.end() )
		return;
	
	WV_FREE( *it );
	m_connectedDevices.erase( it );
}

void wv::XInputControllerDriver::updateDeviceState( InputSystem* _inputSystem, int _deviceID, const XINPUT_STATE& _state )
{
	auto it = getDevice( _deviceID );
	if ( it == m_connectedDevices.end() )
		return;

	ControllerDevice* device = *it;
	ControllerDevice oldState = *device;

	// Update current state

	device->leftJoystick = {
		fmaxf( -1, (float)_state.Gamepad.sThumbLX / 32767 ),
		fmaxf( -1, (float)_state.Gamepad.sThumbLY / 32767 )
	};

	device->rightJoystick = {
		fmaxf( -1, (float)_state.Gamepad.sThumbRX / 32767 ),
		fmaxf( -1, (float)_state.Gamepad.sThumbRY / 32767 )
	};

	// Check state changes
	/*
	std::vector<Action*> actions = _inputSystem->getActionsByDevice( "Controller" );
	

	_inputSystem->pushActionChange("Controller", );
	
	*/

	if ( device->leftJoystick  != oldState.leftJoystick  ) wv::Debug::Print( "LEFT: %f, %f\n", device->leftJoystick.x, device->leftJoystick.y );
	if ( device->rightJoystick != oldState.rightJoystick ) wv::Debug::Print( "RIGHT: %f, %f\n", device->rightJoystick.x, device->rightJoystick.y );

	if ( _state.Gamepad.wButtons & XINPUT_GAMEPAD_A ) device->buttonStates |= CONTROLLER_BUTTON_A; else device->buttonStates &= ~CONTROLLER_BUTTON_A;
	if ( _state.Gamepad.wButtons & XINPUT_GAMEPAD_B ) device->buttonStates |= CONTROLLER_BUTTON_B; else device->buttonStates &= ~CONTROLLER_BUTTON_B;
	if ( _state.Gamepad.wButtons & XINPUT_GAMEPAD_X ) device->buttonStates |= CONTROLLER_BUTTON_X; else device->buttonStates &= ~CONTROLLER_BUTTON_X;
	if ( _state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ) device->buttonStates |= CONTROLLER_BUTTON_Y; else device->buttonStates &= ~CONTROLLER_BUTTON_Y;

	if ( device->buttonStates != oldState.buttonStates )
	{
		uint32_t buttonStateXor = device->buttonStates ^ oldState.buttonStates;

		for ( ActionGroup* group : _inputSystem->getActionGroups() )
		{
			if ( !group->isEnabled() )
				continue;
			
			for ( auto& mapping : group->getTriggerActionsByDevice( "Controller" ) )
			{
				switch ( mapping.inputID )
				{
				case CONTROLLER_BUTTON_A: 
					if ( buttonStateXor & CONTROLLER_BUTTON_A ) handleTriggerAction( _inputSystem, mapping.action, device->buttonStates & CONTROLLER_BUTTON_A ); 
					break;
				case CONTROLLER_BUTTON_B:
					if ( buttonStateXor & CONTROLLER_BUTTON_A ) handleTriggerAction( _inputSystem, mapping.action, device->buttonStates & CONTROLLER_BUTTON_A );
					break;
				case CONTROLLER_BUTTON_X:
					if ( buttonStateXor & CONTROLLER_BUTTON_A ) handleTriggerAction( _inputSystem, mapping.action, device->buttonStates & CONTROLLER_BUTTON_A );
					break;
				case CONTROLLER_BUTTON_Y:
					if ( buttonStateXor & CONTROLLER_BUTTON_A ) handleTriggerAction( _inputSystem, mapping.action, device->buttonStates & CONTROLLER_BUTTON_A );
					break;
				}
			}
		}
	}

}

void wv::XInputControllerDriver::handleTriggerAction( InputSystem* _inputSystem, TriggerAction* _action, bool _state )
{
	if ( _action->currentState == _state )
		return;
	
	_action->currentState = _state;
	//_inputSystem->pushTriggerActionChange( _action, _state );
	
	wv::Debug::Print( "Action %s became state %s\n", _action->name.c_str(), _action->currentState ? "true" : "false" );
}
