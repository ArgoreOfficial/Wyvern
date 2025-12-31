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

			updateDeviceState( i, state );
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

void wv::XInputControllerDriver::updateDeviceState( int _deviceID, const XINPUT_STATE& _state )
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

	if ( device->leftJoystick  != oldState.leftJoystick  ) wv::Debug::Print( "LEFT: %f, %f\n", device->leftJoystick.x, device->leftJoystick.y );
	if ( device->rightJoystick != oldState.rightJoystick ) wv::Debug::Print( "RIGHT: %f, %f\n", device->rightJoystick.x, device->rightJoystick.y );
}
