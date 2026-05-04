#include "xinput_controller_driver.h"

#include <wv/input/input_system.h>

void wv::XInputControllerDriver::setMotorSpeed( uint32_t _vdID, uint16_t _left, uint16_t _right, int _mode )
{
	int userIndex = -1;

	for ( ControllerDevice* device : m_connectedDevices )
	{
		if ( device->vdID != _vdID )
			continue;
		userIndex = device->deviceID;
	}

	if ( userIndex == -1 ) return;

	XINPUT_VIBRATION vibration{};
	vibration.wLeftMotorSpeed = _left;
	vibration.wRightMotorSpeed = _right;
	XInputSetState( userIndex, &vibration );
}

void wv::XInputControllerDriver::pollActions( InputSystem* _inputSystem )
{
	XINPUT_STATE state{};

	for ( size_t i = 0; i < XUSER_MAX_COUNT; i++ )
	{
		if ( XInputGetState( i, &state ) == ERROR_SUCCESS )
		{
			if ( !m_connectedDeviceIDs.contains( i ) ) // device was just connected
				handleDeviceConnected( _inputSystem, i );

			updateDeviceState( _inputSystem, i, state );
		}
		else
		{
			if ( m_connectedDeviceIDs.contains( i ) ) // device was just disconnected
				handleDeviceDisconnected( _inputSystem, i );
		}
	}
}

void wv::XInputControllerDriver::handleDeviceConnected( InputSystem* _inputSystem, int _deviceID )
{
	m_connectedDeviceIDs.insert( _deviceID );
	m_xinputButtonStateMap.emplace( _deviceID, 0 );

	ControllerDevice* device = WV_NEW( ControllerDevice );
	device->deviceID = _deviceID;
	device->vdID = wv::Math::randomU32();

	m_connectedDevices.push_back( device );	

	_inputSystem->pushActionEvent( { ActionType::ActionType_DeviceConnected, device->vdID } );
}

void wv::XInputControllerDriver::handleDeviceDisconnected( InputSystem* _inputSystem, int _deviceID )
{
	m_connectedDeviceIDs.erase( _deviceID );
	m_xinputButtonStateMap.erase( _deviceID );

	auto it = getDevice( _deviceID );
	if ( it == m_connectedDevices.end() )
		return;

	_inputSystem->pushActionEvent( { ActionType::ActionType_DeviceDisconnected, ( *it )->vdID } );

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

	device->leftJoystick  = calibrateDeadzone( device->leftJoystick );
	device->rightJoystick = calibrateDeadzone( device->rightJoystick );

	device->leftTrigger  = (float)_state.Gamepad.bLeftTrigger / 255;
	device->rightTrigger = (float)_state.Gamepad.bRightTrigger / 255;

	updateButtonStates( _inputSystem, device, _state );
	
	if ( device->buttonStates != oldState.buttonStates )
		sendTriggerEvents( _inputSystem, device );
	
	sendAxisEvents( _inputSystem, device, &oldState );
	sendValueEvents( _inputSystem, device, &oldState );
}

void wv::XInputControllerDriver::updateButtonStates( InputSystem* _inputSystem, ControllerDevice* _device, const XINPUT_STATE& _state )
{
	if ( m_xinputButtonStateMap.at( _device->deviceID ) == _state.Gamepad.wButtons )
		return;

	m_xinputButtonStateMap.at( _device->deviceID ) = _state.Gamepad.wButtons;

	_device->setButtonState( CONTROLLER_BUTTON_A, _state.Gamepad.wButtons & XINPUT_GAMEPAD_A );
	_device->setButtonState( CONTROLLER_BUTTON_B, _state.Gamepad.wButtons & XINPUT_GAMEPAD_B );
	_device->setButtonState( CONTROLLER_BUTTON_X, _state.Gamepad.wButtons & XINPUT_GAMEPAD_X );
	_device->setButtonState( CONTROLLER_BUTTON_Y, _state.Gamepad.wButtons & XINPUT_GAMEPAD_Y );
	_device->setButtonState( CONTROLLER_BUTTON_DPAD_UP, _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP );
	_device->setButtonState( CONTROLLER_BUTTON_DPAD_RIGHT, _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT );
	_device->setButtonState( CONTROLLER_BUTTON_DPAD_DOWN, _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN );
	_device->setButtonState( CONTROLLER_BUTTON_DPAD_LEFT, _state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT );

	_device->setButtonState( CONTROLLER_BUTTON_START, _state.Gamepad.wButtons & XINPUT_GAMEPAD_START );
	_device->setButtonState( CONTROLLER_BUTTON_SELECT, _state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK );
	_device->setButtonState( CONTROLLER_BUTTON_HOME, false );

	_device->setButtonState( CONTROLLER_BUTTON_JOYSTICK_LEFT, _state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB );
	_device->setButtonState( CONTROLLER_BUTTON_JOYSTICK_RIGHT, _state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB );
	_device->setButtonState( CONTROLLER_BUTTON_SHOULDER_LEFT, _state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER );
	_device->setButtonState( CONTROLLER_BUTTON_SHOULDER_RIGHT, _state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER );

}
