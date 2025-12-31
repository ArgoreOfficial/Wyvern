#pragma once

#include <wv/input/drivers/controller_driver.h>

#include <Xinput.h>

namespace wv {

class TriggerAction;

class XInputControllerDriver : public IControllerDriver
{
public:
	XInputControllerDriver() = default;

protected:
	void updateDriver( InputSystem* _inputSystem ) override;
	void handleDeviceConnected( int _deviceID );
	void handleDeviceDisconnected( int _deviceID );

	void updateDeviceState( InputSystem* _inputSystem, int _deviceID, const XINPUT_STATE& _state );

	void handleTriggerAction( InputSystem* _inputSystem, TriggerAction* _action, bool _state );
};

}