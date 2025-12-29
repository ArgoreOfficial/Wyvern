#include "button_action.h"

#include <wv/debug/log.h>

void wv::ButtonAction::handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown )
{
	if ( !isScancodeBound( _scancode ) ) return;

	if ( !m_isDown && _keyDown )
		wv::Debug::Print( "The action %s happened\n", m_name.c_str() );

	m_isDown = _keyDown;
}

void wv::ButtonAction::handleControllerEvent( uint32_t _button, bool _buttonDown )
{
	if ( !isControllerButtonBound( _button ) ) return;

	if ( !m_isDown && _buttonDown )
		wv::Debug::Print( "The action %s happened\n", m_name.c_str() );

	m_isDown = _buttonDown;
}
