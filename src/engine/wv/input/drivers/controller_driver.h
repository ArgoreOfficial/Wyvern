#pragma once

#include <wv/input/drivers/input_driver.h>
#include <wv/input/input_enums.h>

#include <wv/math/vector2.h>
#include <wv/memory/memory.h>

#include <vector>
#include <set>

namespace wv {

struct TriggerAction;

struct ControllerDevice
{
	int deviceID = -1;

	wv::Vector2f leftJoystick;
	wv::Vector2f rightJoystick;

	// Mask, use ControllerButton enum for bits 
	uint32_t buttonStates = CONTROLLER_BUTTON_NONE;

	inline bool getButtonState( ControllerButton _button ) {
		return ( buttonStates & _button ) != 0;
	}

	inline void setButtonState( ControllerButton _button, bool _state ) {
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

	virtual void updateDriver( InputSystem* _inputSystem ) = 0;

	void handleTriggerAction( InputSystem* _inputSystem, TriggerAction* _action, bool _state );

	virtual void sendTriggerEvents( InputSystem* _inputSystem, ControllerDevice* _device );

	std::set<int> m_connectedDeviceIDs;
	std::vector<ControllerDevice*> m_connectedDevices;
};

}