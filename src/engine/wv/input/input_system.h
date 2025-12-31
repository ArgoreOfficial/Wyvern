#pragma once

#include <wv/debug/log.h>
#include <wv/debug/error.h>

#include <wv/event/event_listener.h>
#include <wv/math/vector2.h>
#include <wv/input/action_group.h>
#include <wv/input/input_enums.h>

#include <vector>

namespace wv {

class IInputDriver;
class EventManager;
class MouseMoveEvent;
class MouseButtonEvent;
class KeyboardEvent;
class ControllerButtonEvent;

class InputSystem
{
public:
	InputSystem();
	~InputSystem();
	
	template<typename Ty>
	void createInputDriver() {
		static_assert( std::is_base_of<IInputDriver, Ty>(), "Must be a valid IInputDriver" );
		m_inputDrivers.push_back( WV_NEW( Ty ) );
		// driver->initialize() ?
	}

	void updateInputDrivers( EventManager* _eventManager );
	void processInputEvents( EventManager* _eventManager );

	ActionGroup* getActionGroup( const std::string& _name ) const { 
		if ( !m_actionGroupNameMap.contains( _name ) )
		{
			WV_LOG_ERROR( "There is no action group named '%s'\n", _name.c_str() );
			return nullptr;
		}
		return m_actionGroupNameMap.at( _name ); 
	}
	ActionGroup* createActionGroup( const std::string& _name );
	void destroyActionGroup( const std::string& _name );
	
	std::vector<IAction*> getActionQueue() { return m_actionQueue; }

	void postActionEvent( IAction* _action ) { m_actionQueue.push_back( _action ); }
	
#ifndef WV_PACKAGE
	inline wv::Vector2f debugGetMouseMotion()   const { return m_debugMouseMotion; }
	inline wv::Vector2f debugGetMousePosition() const { return m_debugMousePosition; }
	
	inline bool debugIsMouseDown( int _index ) {
		WV_ASSERT( _index < 0 );
		WV_ASSERT( _index >= 5 );
		return m_debugMouseButtonStates[ _index ];
	}
#endif

protected:
	void onMouseMoveEvent( const MouseMoveEvent& _event );
	void onMouseButtonEvent( const MouseButtonEvent& _event );
	void onKeyboardEvent( const KeyboardEvent& _event );
	void onControllerButtonEvent( const ControllerButtonEvent& _event );

#ifndef WV_PACKAGE
	wv::Vector2f m_debugMouseMotion{ 0.0f, 0.0f };
	wv::Vector2f m_debugMousePosition{ 0.0f, 0.0f };
	bool m_debugMouseButtonStates[ 5 ] = { false, false, false, false, false };
#endif
	
	std::vector<IInputDriver*> m_inputDrivers;

	std::vector<IAction*> m_actionQueue;

	std::vector<ActionGroup*> m_actionGroups;
	std::unordered_map<std::string, ActionGroup*> m_actionGroupNameMap;

	uint32_t m_mouseMoveListener        = 0;
	uint32_t m_mouseButtonListener      = 0;
	uint32_t m_keyboardListener         = 0;
	uint32_t m_controllerButtonListener = 0;

};

}