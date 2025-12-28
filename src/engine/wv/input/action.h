#pragma once

#include <wv/reflection/reflection.h>

namespace wv {

class IAction : public IReflectedType
{
	friend class ActionGroup;

	WV_REFLECT_TYPE( IAction )
public:
	IAction( const std::string& _name ) : m_name{ _name } { }

	virtual void handleKeyboardEvent( uint32_t _scancode, bool _keyDown ) = 0;
	virtual void handleControllerEvent( uint32_t _button, bool _buttonDown ) = 0;

	virtual bool isBoundToKeyboard() const = 0;
	virtual bool isBoundToController() const = 0;
	virtual bool isBoundToMouse() const = 0;

	bool requiresRemapping() const { return m_requiresRemapping; };

protected:
	std::string m_name;
	bool m_requiresRemapping = true;
};

}