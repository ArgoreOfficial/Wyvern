#include "button_action.h"

#include <wv/debug/log.h>

#include <wv/application.h>
#include <wv/event/event_manager.h>

void wv::ButtonAction::handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown )
{
	if ( !isScancodeBound( _scancode ) ) return;

	if ( !m_isDown && _keyDown )
		publishEvent( _keyDown );
		
	m_isDown = _keyDown;

}

void wv::ButtonAction::handleControllerEvent( wv::ControllerButton _button, bool _buttonDown )
{
	if ( !isControllerButtonBound( _button ) ) return;

	if ( !m_isDown && _buttonDown )
		publishEvent( _buttonDown );
		
	m_isDown = _buttonDown;
}

void wv::ButtonAction::publishEvent( bool _state )
{
	ButtonActionEvent event;
	event.actionID = m_actionID;
	event.state = _state;
	wv::Application::getSingleton()->getEventManager()->queueEvent( event );
}