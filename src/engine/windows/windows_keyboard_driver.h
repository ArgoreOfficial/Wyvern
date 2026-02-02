#pragma once

#include <wv/input/drivers/keyboard_driver.h>

namespace wv {

class WindowsKeyboardDriver : public IKeyboardDriver
{
	WV_REFLECT_TYPE( WindowsKeyboardDriver, IKeyboardDriver )
public:
	WindowsKeyboardDriver() = default;

protected:
	virtual void pollActions( InputSystem* _inputSystem ) override;

};

}