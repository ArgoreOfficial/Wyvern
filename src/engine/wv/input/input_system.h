#pragma once

#include <wv/debug/log.h>
#include <wv/debug/error.h>

#include <wv/input/action_group.h>
#include <wv/input/input_enums.h>
#include <wv/math/vector2.h>

#include <vector>
#include <set>
#include <string>
#include <queue>

namespace wv {

class IInputDriver;
class IMouseDriver;
class EventManager;

struct MouseState
{
	Vector2i position = {};
	Vector2i motion = {};
	bool buttonStates[ 5 ] = {};
	int scrollDelta = 0;
};

struct VirtualDevice
{
	uint32_t virtualDeviceID = 0;
	int playerIndex = -1;
};

struct ActionEvent
{
	ActionType type = ActionType_Trigger;
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

class LowLevelInputQueue
{
public:
	enum EventType
	{
		EventType_Move,
		EventType_Button,
		EventType_Scroll
	};

	struct Event
	{
		EventType type;

		union
		{
			struct MouseMoveEvent
			{
				wv::Vector2f position;
				wv::Vector2f delta;
			} move;

			struct MouseButtonEvent
			{
				int index;
				bool state;
			} button;

			struct MouseScrollEvent
			{
				int delta;
			} scroll;
		} mouse;
	};

	void pushEvent( const Event& _event ) {
		m_events.push_back( _event );
	}

	const std::vector<LowLevelInputQueue::Event>& getQueue() {
		return m_events;
	}

	void clearQueue() {
		m_events.clear();
	}

private:
	std::vector<LowLevelInputQueue::Event> m_events;
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void initialize( IMouseDriver* _mouseDriver );
	void shutdown();
	
	template<typename Ty>
	Ty* createInputDriver() {
		static_assert( std::is_base_of<IInputDriver, Ty>(), "Must be a valid IInputDriver" );
		Ty* p = WV_NEW( Ty );
		m_inputDrivers.push_back( p );
		return p;
	}

	void processInputEvents( EventManager* _eventManager );

	void setMotorSpeed( uint32_t _vdID, uint16_t _left, uint16_t _right );

	void setDevicePlayer( uint32_t _vdID, int _playerIndex );
	int  getDevicePlayer( uint32_t _vdID );
	
	void pushActionEvent( const ActionEvent& _event ) { m_actionEventQueue.push_back( _event ); }
	void pushActionEvent( TriggerAction* _action, uint32_t _vdID ) { pushActionEvent( { ActionType_Trigger, _vdID, getDevicePlayer( _vdID ), _action->actionID, _action } ); }
	void pushActionEvent( ValueAction*   _action, uint32_t _vdID ) { pushActionEvent( { ActionType_Value,   _vdID, getDevicePlayer( _vdID ), _action->actionID, _action } ); }
	void pushActionEvent( AxisAction*    _action, uint32_t _vdID ) { pushActionEvent( { ActionType_Axis,    _vdID, getDevicePlayer( _vdID ), _action->actionID, _action } ); }

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
	
	/**
	 * @brief Enable or Disable actions globally
	 */
	void setActionsEnabled( bool _enabled ) { m_shouldSendActionEvents = _enabled; }
	bool getActionsEnabled()          const { return m_shouldSendActionEvents; }

	const std::vector<ActionGroup*>& getActionGroups() const { return m_actionGroups; }

	inline Vector2i getMouseMotion()   const { return m_mouseState.motion; }
	inline Vector2i getMousePosition() const { return m_mouseState.position; }
	inline int      getMouseScroll()   const { return m_mouseState.scrollDelta; }

	inline bool getMouseButtonState( int _index ) const {
		WV_ASSERT( _index >= 0 && _index < 5 );
		return m_mouseState.buttonStates[ _index ];
	}
	
	inline bool getMouseButtonDown( int _index ) const {
		WV_ASSERT( _index >= 0 && _index < 5 );
		return !m_prevMouseState.buttonStates[ _index ] && m_mouseState.buttonStates[ _index ];
	}

	inline bool getMouseButtonUp( int _index ) const {
		WV_ASSERT( _index >= 0 && _index < 5 );
		return m_prevMouseState.buttonStates[ _index ] && !m_mouseState.buttonStates[ _index ];
	}

	LowLevelInputQueue& getLowLevelQueue() { return m_lowLevelInputQueue; }

protected:
	
	MouseState m_mouseState{};
	MouseState m_prevMouseState{};

	LowLevelInputQueue m_lowLevelInputQueue{};

	std::vector<IInputDriver*> m_inputDrivers;
	
	std::vector<ActionGroup*> m_actionGroups;
	std::unordered_map<std::string, ActionGroup*> m_actionGroupNameMap;

	std::vector<VirtualDevice> m_virtualDevices;

	std::vector<ActionEvent> m_actionEventQueue;

	IMouseDriver* m_mouseDriver = nullptr;

	bool m_shouldSendActionEvents = true;
};

}