#pragma once

#include <wv/debug/error.h>
#include <wv/memory/memory.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <wv/math/vector2.h>

namespace wv {

class IAction;
class ButtonAction;
class AxisAction;

class ActionGroup
{
public:
	ActionGroup( const std::string& _name ) 
		: m_name{ _name } 
	{ }
	
	~ActionGroup();

	inline bool isEnabled() const { return m_isEnabled; }

	void enable();
	void disable() {
		WV_ASSERT( m_isEnabled == false );
		m_isEnabled = false;
	}

	std::string getName() const { return m_name; }

	ButtonAction* createButtonAction( const std::string& _name );
	AxisAction* createAxisAction( const std::string& _name );

	void destroyAction( const std::string& _name );

	void handleKeyboardEvent( uint32_t _scancode, bool _keyDown );
	void handleControllerEvent( uint32_t _button, bool _buttonDown );

	wv::Vector2<double> getAxisValue( const std::string& _name ) const;

private:
	IAction* createAction( IAction* _action );

	void buildActionMapping();

	std::string m_name;

	bool m_isEnabled = false;
	bool m_requiresRemapping = false;

	std::vector<IAction*> m_actions;
	std::unordered_map<std::string, IAction*> m_actionNameMap;

	std::vector<IAction*> m_keyboardBoundActions;
	std::vector<IAction*> m_controllerBoundActions;
	std::vector<IAction*> m_mouseBoundActions;
};

}