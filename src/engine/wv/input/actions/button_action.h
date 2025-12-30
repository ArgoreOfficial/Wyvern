#pragma once

#include <wv/input/action.h>
#include <wv/event/event.h>
#include <wv/event/event_listener.h>
#include <wv/memory/memory.h>

#include <set>
#include <wv/helpers/unordered_array.hpp>

namespace wv {

class EventManager;

template<typename Ty>
class ActionEventListener : public IEventListener
{
	static_assert( std::is_base_of<IEvent, Ty>(), "Must be a valid IEvent type" );
	friend class EventManager;

public:
	typedef std::function<void( const Ty& _event )> EventFunction_t;

	ActionEventListener() :
		IEventListener( Ty::getStaticTypeUUID() )
	{
	}

	ActionEventListener( const EventFunction_t& _function ) :
		IEventListener( Ty::getStaticTypeUUID() ),
		m_eventFunction{ _function }
	{
	}

	void setAction( IAction* _action ) { m_actionID = _action->getActionID(); }

protected:
	virtual void triggerEvent( const IEvent& _event ) {
		const Ty& event = static_cast<const Ty&>( _event );
		if ( event.actionID == m_actionID )
			m_eventFunction( event );
	}

private:
	EventFunction_t m_eventFunction;
	uint64_t m_actionID = 0;
};

class ButtonOnPressedEvent : public IEvent
{
	WV_REFLECT_TYPE( ButtonOnPressedEvent )
public:
	uint64_t actionID = 0;
};

class ButtonOnReleasedEvent : public IEvent
{
	WV_REFLECT_TYPE( ButtonOnReleasedEvent )
public:
	uint64_t actionID = 0;
};

class ButtonOnStateChangedEvent : public IEvent
{
	WV_REFLECT_TYPE( ButtonOnStateChangedEvent )
public:
	uint64_t actionID = 0;
	bool state = false;
};

// 

class ButtonAction : public IAction
{
	WV_REFLECT_TYPE( ButtonAction )
public:
	ButtonAction( const std::string& _name );

	virtual void handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown ) override {
		if ( !isScancodeBound( _scancode ) ) return;
		updateState( _keyDown );
	}

	virtual void handleControllerEvent( wv::ControllerButton _button, bool _buttonDown ) override {
		if ( !isControllerButtonBound( _button ) ) return;
		updateState( _buttonDown );
	}

	virtual bool isBoundToKeyboard() const override { return !m_boundScancodes.empty(); }
	virtual bool isBoundToController() const override { return !m_boundControllerButtons.empty(); }
	virtual bool isBoundToMouse() const override { return false; }

	inline bool isScancodeBound( wv::Scancode _scancode ) const { return m_boundScancodes.contains( _scancode ); }
	inline bool isControllerButtonBound( wv::ControllerButton _button ) const { return m_boundControllerButtons.contains( _button ); }

	inline void bindScancode( wv::Scancode _scancode ) {
		if ( isScancodeBound( _scancode ) ) return;
		m_boundScancodes.insert( _scancode );
		m_requiresRemapping = true;
	}

	inline void unbindScancode( wv::Scancode _scancode ) {
		if ( !isScancodeBound( _scancode ) ) return;
		m_boundScancodes.erase( _scancode );
		m_requiresRemapping = true;
	}

	inline void bindControllerButton( wv::ControllerButton _button ) {
		if ( isControllerButtonBound( _button ) ) return;
		m_boundControllerButtons.insert( _button );
		m_requiresRemapping = true;
	}

	inline void unbindControllerButton( wv::ControllerButton _button ) {
		if ( !isControllerButtonBound( _button ) ) return;
		m_boundControllerButtons.erase( _button );
		m_requiresRemapping = true;
	}

	uint32_t subscribeOnPressed( std::function<void( const ButtonOnPressedEvent& _event )> _callback );
	void unsubscribeOnPressed( uint32_t _listenerID );

private:
	void updateState( bool _newState );
	
	void onPressed();
	void onReleased();
	void onStateChanged();

	std::set<wv::Scancode> m_boundScancodes;
	std::set<wv::ControllerButton> m_boundControllerButtons;
	bool m_state = false;

	wv::unordered_array<uint32_t, ActionEventListener<ButtonOnPressedEvent>*> m_onPressedListener;
	wv::unordered_array<uint32_t, ActionEventListener<ButtonOnReleasedEvent>*> m_onReleasedListener;
	wv::unordered_array<uint32_t, ActionEventListener<ButtonOnStateChangedEvent>*> m_onStateChangedListener;

	EventManager* m_eventManager = nullptr;
};

}