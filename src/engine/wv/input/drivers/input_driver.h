#pragma once
#include <wv/math/vector2.h>

namespace wv {

class InputSystem;

struct TriggerAction;
struct AxisAction;
struct ValueAction;

enum AxisActionDirection;

class IInputDriver
{
	friend class InputSystem;

public:
	IInputDriver() = default;
	virtual ~IInputDriver() { }

protected:
	virtual void updateDriver( InputSystem* _inputSystem ) = 0;

	void handleTriggerAction( InputSystem* _inputSystem, TriggerAction* _action, bool _state );
	void handleValueAction( InputSystem* _inputSystem, ValueAction* _action, float _value );
	void handleAxisAction( InputSystem* _inputSystem, AxisAction* _action, AxisActionDirection _direction, const wv::Vector2f& _value, bool _additive = false );

};

}