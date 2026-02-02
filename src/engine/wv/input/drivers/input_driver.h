#pragma once

#include <wv/math/vector2.h>
#include <wv/reflection/reflection.h>

namespace wv {

class InputSystem;

struct TriggerAction;
struct AxisAction;
struct ValueAction;

enum AxisActionDirection;

class IInputDriver : public IReflectedType
{
	friend class InputSystem;
	WV_REFLECT_TYPE( IInputDriver, IReflectedType )

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
	void handleAxisAction( InputSystem* _inputSystem, uint32_t _vdID, AxisAction* _action, AxisActionDirection _direction, const wv::Vector2f& _value, bool _additive = false );

};

}