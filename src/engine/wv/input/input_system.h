#pragma once

#include <wv/debug/log.h>
#include <wv/debug/error.h>

#include <wv/math/vector2.h>
#include <wv/input/action_group.h>
#include <wv/input/input_enums.h>

#include <vector>

namespace wv {

class IInputDriver;
class EventManager;

struct ActionEvent
{
	ActionType type = ACTION_TYPE_TRIGGER;
	ActionID actionID = 0;
	union ActionEventValue
	{
		const TriggerAction* trigger = nullptr;
		const ValueAction* value;
		const AxisAction* axis;
	} action{};
};

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

	void pushActionEvent( TriggerAction* _action ) {
		ActionEvent event{ ACTION_TYPE_TRIGGER, _action->actionID };
		event.action.trigger = _action;
		m_actionEventQueue.push_back( event );
	}

	void pushActionEvent( ValueAction* _action ) {
		ActionEvent event{ ACTION_TYPE_VALUE, _action->actionID };
		event.action.value = _action;
		m_actionEventQueue.push_back( event );
	}

	void pushActionEvent( AxisAction* _action ) {
		ActionEvent event{ ACTION_TYPE_AXIS, _action->actionID };
		event.action.axis = _action;
		m_actionEventQueue.push_back( event );
	}

	std::vector<ActionEvent> getActionEventQueue() const {
		return m_actionEventQueue;
	}

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
	
	const std::vector<ActionGroup*>& getActionGroups() const { return m_actionGroups; }

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
#ifndef WV_PACKAGE
	wv::Vector2f m_debugMouseMotion{ 0.0f, 0.0f };
	wv::Vector2f m_debugMousePosition{ 0.0f, 0.0f };
	bool m_debugMouseButtonStates[ 5 ] = { false, false, false, false, false };
#endif
	
	std::vector<IInputDriver*> m_inputDrivers;

	std::vector<ActionGroup*> m_actionGroups;
	std::unordered_map<std::string, ActionGroup*> m_actionGroupNameMap;

	std::vector<ActionEvent> m_actionEventQueue;
};

}