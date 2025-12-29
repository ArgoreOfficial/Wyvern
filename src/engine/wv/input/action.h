#pragma once

#include <wv/reflection/reflection.h>
#include <wv/input/input_enums.h>

namespace wv {

class IAction : public IReflectedType
{
	friend class ActionGroup;

	WV_REFLECT_TYPE( IAction )
public:
	IAction( const std::string& _name ) : m_name{ _name } { }

	virtual void handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown ) { };
	virtual void handleControllerEvent( uint32_t _button, bool _buttonDown ) { };
	virtual void handleJoystickEvent( float _x, float _y, float _relativeX, float _relativeY ) { };

	virtual bool isBoundToKeyboard() const = 0;
	virtual bool isBoundToController() const = 0;
	virtual bool isBoundToMouse() const = 0;

	bool requiresRemapping() const { return m_requiresRemapping; };

protected:
	std::string m_name;
	bool m_requiresRemapping = true;
};

}