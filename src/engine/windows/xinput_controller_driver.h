#pragma once

#include <wv/input/drivers/controller_driver.h>

#include <Xinput.h>

namespace wv {

class XInputControllerDriver : public IControllerDriver
{
	WV_REFLECT_TYPE( XInputControllerDriver, IInputDriver )
public:
	XInputControllerDriver() = default;

	virtual void setMotorSpeed( uint32_t _vdID, uint16_t _left, uint16_t _right, int _mode ) override;

protected:
	virtual void pollActions( InputSystem* _inputSystem ) override;

	void handleDeviceConnected( InputSystem* _inputSystem, int _deviceID );
	void handleDeviceDisconnected( InputSystem* _inputSystem, int _deviceID );

	void updateDeviceState( InputSystem* _inputSystem, int _deviceID, const XINPUT_STATE& _state );

	void updateButtonStates( InputSystem* _inputSystem, ControllerDevice* _device, const XINPUT_STATE& _state );

	std::unordered_map<int, WORD> m_xinputButtonStateMap;
};

}