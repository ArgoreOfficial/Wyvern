#pragma once

#include <wv/input/drivers/input_driver.h>
#include <wv/input/input_enums.h>

#include <wv/math/vector2.h>
#include <wv/memory/memory.h>

#include <vector>
#include <set>

namespace wv {

struct ControllerDevice
{
	int deviceID = -1;
	uint32_t vdID = 0; // virtual device ID, not tied to real device ID

	wv::Vector2f leftJoystick;
	wv::Vector2f rightJoystick;

	float leftTrigger = 0.0f;
	float rightTrigger = 0.0f;

	// Mask, use ControllerInputs enum for bits 
	uint32_t buttonStates = CONTROLLER_BUTTON_NONE;

	inline bool getButtonState( ControllerInputs _button ) {
		return ( buttonStates & _button ) != 0;
	}

	inline void setButtonState( ControllerInputs _button, bool _state ) {
		if ( _state )
			buttonStates |= _button;
		else
			buttonStates &= ~_button;
	}
};

class IControllerDriver : public IInputDriver
{
public:
	virtual ~IControllerDriver() {
		for ( ControllerDevice* device : m_connectedDevices )
			WV_FREE( device );
	}
	
	virtual void setRumble( uint32_t _vdID, uint16_t _left, uint16_t _right, int _mode ) = 0;

protected:
	std::vector<ControllerDevice*>::iterator getDevice( int _deviceID ) {
		for ( auto it = m_connectedDevices.begin(); it != m_connectedDevices.end(); )
		{
			if ( ( *it )->deviceID != _deviceID )
				continue;
			return it;
		}

		return m_connectedDevices.end();
	}

	virtual void pollActions( InputSystem* _inputSystem ) = 0;
	
	virtual void sendTriggerEvents( InputSystem* _inputSystem, ControllerDevice* _device );
	virtual void sendValueEvents( InputSystem* _inputSystem, ControllerDevice* _device, ControllerDevice* _prevDeviceState );
	virtual void sendAxisEvents( InputSystem* _inputSystem, ControllerDevice* _device, ControllerDevice* _prevDeviceState );

	std::set<int> m_connectedDeviceIDs;
	std::vector<ControllerDevice*> m_connectedDevices;
};

}