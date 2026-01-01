#pragma once

#include <wv/reflection/reflection.h>
#include <wv/input/input_enums.h>
#include <wv/math/math.h>

namespace wv {

class IAction : public IReflectedType
{
	friend class ActionGroup;

	WV_REFLECT_TYPE( IAction )
public:
	IAction( const std::string& _name ) : m_name{ _name } { }

	virtual void handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown ) { }
	virtual void handleControllerEvent( wv::ControllerInputs _button, bool _buttonDown ) { }
	virtual void handleJoystickEvent( float _x, float _y, float _relativeX, float _relativeY ) { }

	virtual bool isBoundToKeyboard() const = 0;
	virtual bool isBoundToController() const = 0;
	virtual bool isBoundToMouse() const = 0;

	bool requiresRemapping() const { return m_requiresRemapping; };
	uint64_t getActionID() const { return m_actionID; }

protected:
	std::string m_name;
	const uint64_t m_actionID = wv::Math::randomU64();
	bool m_requiresRemapping = true;
};

}