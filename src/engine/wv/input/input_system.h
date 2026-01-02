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

struct ActionEvent
{
	ActionType type = ACTION_TYPE_TRIGGER;
	uint32_t vdID = 0;
	int playerIndex = -1;
	ActionID actionID = 0;

	union ActionEventValue
	{
		TriggerAction* trigger = nullptr;
		ValueAction* value;
		AxisAction* axis;
	} action{};
};

struct VirtualDevice
{
	uint32_t virtualDeviceID = 0;
	IInputDriver* driver = nullptr;
	std::string deviceType;
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

	void mapNextAvailableDeviceToPlayer( int _playerIndex );

	void updateInputDrivers( EventManager* _eventManager );
	void processInputEvents( EventManager* _eventManager );

	// Virtual Device

	uint32_t requestVirtualDeviceID( IInputDriver* _driver, const std::string& _deviceType ) {
		uint32_t vdID = 1;
		while ( m_virtualDeviceIDs.contains( vdID ) )
			vdID++;

		m_virtualDeviceIDs.insert( vdID );
		m_virtualDevices[ vdID ] = { vdID, _driver, _deviceType };
		return vdID;
	}

	void releaseVirtualDeviceID( uint32_t _vdID ) {
		if ( !m_virtualDeviceIDs.contains( _vdID ) )
			return;
		m_virtualDeviceIDs.erase( _vdID );
		m_virtualDevices[ _vdID ] = {};
	}

	void setControllerRumble( uint32_t _vdID, uint16_t _left, uint16_t _right );

	// Players

	void mapVirtualDeviceToPlayer( uint32_t _vdID, int _playerIndex ) {
		if ( !m_virtualDeviceIDs.contains( _vdID ) ) return;
		m_vdPlayerMap[ _vdID ] = _playerIndex;
	}

	void unmapPlayer( int _playerIndex ) {
		auto it = m_vdPlayerMap.find( _playerIndex );
		if ( it == m_vdPlayerMap.end() ) return;
		m_vdPlayerMap.erase( it );
	}

	int getMappedPlayerIndex( uint32_t _vdID ) {
		if ( m_vdPlayerMap.contains( _vdID ) )
			return m_vdPlayerMap.at( _vdID );
		return -1;
	}

	// Action Events

	void pushActionEvent( TriggerAction* _action, uint32_t _vdID ) {
		ActionEvent event{ ACTION_TYPE_TRIGGER, _vdID, getMappedPlayerIndex( _vdID ), _action->actionID };
		event.action.trigger = _action;
		m_actionEventQueue.push_back( event );
	}

	void pushActionEvent( ValueAction* _action, uint32_t _vdID ) {
		ActionEvent event{ ACTION_TYPE_VALUE, _vdID, getMappedPlayerIndex( _vdID ), _action->actionID };
		event.action.value = _action;
		m_actionEventQueue.push_back( event );
	}

	void pushActionEvent( AxisAction* _action, uint32_t _vdID ) {
		ActionEvent event{ ACTION_TYPE_AXIS, _vdID, getMappedPlayerIndex( _vdID ), _action->actionID };
		event.action.axis = _action;
		m_actionEventQueue.push_back( event );
	}

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
	std::queue<int> m_playerDeviceMapQueue; // mapNextAvailableDeviceToPlayer

	std::vector<ActionGroup*> m_actionGroups;
	std::unordered_map<std::string, ActionGroup*> m_actionGroupNameMap;

	std::vector<ActionEvent> m_actionEventQueue;

	std::set<uint32_t> m_virtualDeviceIDs;
	std::unordered_map<uint32_t, int> m_vdPlayerMap;
	std::unordered_map<uint32_t, VirtualDevice> m_virtualDevices;
};

}