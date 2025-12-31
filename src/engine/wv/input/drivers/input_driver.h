#pragma once

namespace wv {

class InputSystem;

class IInputDriver
{
	friend class InputSystem;

public:
	IInputDriver() = default;
	virtual ~IInputDriver() { }

protected:
	virtual void updateDriver( InputSystem* _inputSystem ) = 0;
};

}