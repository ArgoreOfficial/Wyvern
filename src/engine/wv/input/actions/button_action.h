#pragma once

#include <wv/input/action.h>
#include <wv/event/event.h>
#include <wv/event/event_listener.h>

#include <set>

namespace wv {

class ButtonAction : public IAction
{
	WV_REFLECT_TYPE( ButtonAction )
public:
	ButtonAction( const std::string& _name ) : IAction( _name ) { }

	virtual void handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown ) override;
	virtual void handleControllerEvent( wv::ControllerButton _button, bool _buttonDown ) override;

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

private:
	void publishEvent( bool _state );

	std::set<wv::Scancode> m_boundScancodes;
	std::set<wv::ControllerButton> m_boundControllerButtons;
	bool m_isDown = false;

};

// Events

class ButtonActionEvent : public IEvent
{
	WV_REFLECT_TYPE( ButtonActionEvent )
public:
	uint64_t actionID = 0;
	bool state = false;
};

class ButtonActionEventListener : public IEventListener
{
	friend class EventManager;

public:
	typedef std::function<void( const ButtonActionEvent& _event )> EventFunction_t;

	ButtonActionEventListener() :
		IEventListener( ButtonActionEvent::getStaticTypeUUID() )
	{
	}

	ButtonActionEventListener( const EventFunction_t& _function ) :
		IEventListener( ButtonActionEvent::getStaticTypeUUID() ),
		m_eventFunction{ _function }
	{
	}

	void setAction( ButtonAction* _buttonAction ) {
		m_buttonActionID = _buttonAction->getActionID();
	}

protected:
	virtual void triggerEvent( const IEvent& _event ) {
		const ButtonActionEvent& event = static_cast<const ButtonActionEvent&>( _event );
		if( event.actionID == m_buttonActionID )
			m_eventFunction( event );
	}

private:
	EventFunction_t m_eventFunction;
	uint64_t m_buttonActionID = 0;
};




}