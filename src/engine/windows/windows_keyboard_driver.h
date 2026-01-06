#pragma once

#include <wv/input/drivers/keyboard_driver.h>

namespace wv {

class WindowsKeyboardDriver : public IKeyboardDriver
{
public:
	WindowsKeyboardDriver() = default;

protected:
	virtual void pollActions( InputSystem* _inputSystem ) override;

};

}