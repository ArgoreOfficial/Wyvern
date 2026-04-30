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

	virtual const std::string getDriverType() const = 0;

protected:
	virtual void initialize( InputSystem* _inputSystem ) { }
	virtual void shutdown( InputSystem* _inputSystem ) { }
	virtual void pollActions( InputSystem* _inputSystem ) = 0;

	void handleTriggerAction( InputSystem* _inputSystem, uint32_t _vdID, TriggerAction* _action, bool _state );
	void handleValueAction( InputSystem* _inputSystem, uint32_t _vdID, ValueAction* _action, float _value );
	void handleAxisAction( InputSystem* _inputSystem, uint32_t _vdID, AxisAction* _action, AxisActionDirection _direction, const wv::Vector2f& _value, bool _sendEvent, bool _additive = false );

};

}