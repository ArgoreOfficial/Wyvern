#pragma once

#include <wv/debug/log.h>
#include <wv/debug/error.h>

#include <wv/math/vector2.h>
#include <wv/input/action_group.h>
#include <wv/input/input_enums.h>

#include <vector>
#include <set>
#include <string>
#include <queue>

namespace wv {

class IInputDriver;
class EventManager;

struct VirtualDevice
{
	uint32_t virtualDeviceID = 0;
	int playerIndex = -1;
};

struct ActionEvent
{
	ActionType type = ACTION_TYPE_TRIGGER;
	uint32_t vdID = 0;
	int playerIndex = -1;
	ActionID actionID = 0;

	union ActionEventValue
	{
		void* ptr = nullptr;
		TriggerAction* trigger;
		ValueAction* value;
		AxisAction* axis;
	} action{};
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void initialize();
	void shutdown();
	
	template<typename Ty>
	void createInputDriver() {
		static_assert( std::is_base_of<IInputDriver, Ty>(), "Must be a valid IInputDriver" );
		m_inputDrivers.push_back( WV_NEW( Ty ) );
	}

	// will return the *first* valid driver of type Ty
	template<typename Ty>
	Ty* getInputDriver() {
		static_assert( std::is_base_of<IInputDriver, Ty>(), "Must be a valid IInputDriver" );

		for ( auto it = m_inputDrivers.begin(); it != m_inputDrivers.end(); it++ )
			if ( Ty* driver = tryCast<Ty>( *it ) )
				return driver;
		
		return nullptr;
	}

	void processInputEvents( EventManager* _eventManager );

	void setMotorSpeed( uint32_t _vdID, uint16_t _left, uint16_t _right );

	void setDevicePlayer( uint32_t _vdID, int _playerIndex );
	int  getDevicePlayer( uint32_t _vdID );
	
	void pushActionEvent( const ActionEvent& _event ) { m_actionEventQueue.push_back( _event ); }
	void pushActionEvent( TriggerAction* _action, uint32_t _vdID ) { pushActionEvent( { ACTION_TYPE_TRIGGER, _vdID, getDevicePlayer( _vdID ), _action->actionID, _action } ); }
	void pushActionEvent( ValueAction*   _action, uint32_t _vdID ) { pushActionEvent( { ACTION_TYPE_VALUE,   _vdID, getDevicePlayer( _vdID ), _action->actionID, _action } ); }
	void pushActionEvent( AxisAction*    _action, uint32_t _vdID ) { pushActionEvent( { ACTION_TYPE_AXIS,    _vdID, getDevicePlayer( _vdID ), _action->actionID, _action } ); }

	std::vector<ActionEvent> getActionEventQueue() const {
		return m_actionEventQueue;
	}

	// Action Group

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
		WV_ASSERT( _index >= 0 );
		WV_ASSERT( _index < 5 );
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

	std::vector<VirtualDevice> m_virtualDevices;

	std::vector<ActionEvent> m_actionEventQueue;

};

}