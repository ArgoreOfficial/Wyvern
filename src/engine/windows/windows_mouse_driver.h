#pragma once

#include <wv/input/drivers/mouse_driver.h>

#include <windows.h>

namespace wv {

class WindowsMouseDriver : public IMouseDriver
{
public:
	WindowsMouseDriver() = default;


protected:
	virtual void initialize( InputSystem* _inputSystem ) override;
	virtual void shutdown( InputSystem* _inputSystem ) override;

	virtual void pollActions( InputSystem* _inputSystem ) override;

	HHOOK m_hMouseHook = 0;
};

}