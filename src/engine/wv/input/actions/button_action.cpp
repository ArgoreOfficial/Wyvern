#include "button_action.h"

#include <wv/application.h>
#include <wv/input/input_system.h>

void wv::ButtonAction::updateState( bool _newState )
{
	bool oldState = m_state;
	m_state = _newState;
	
	if ( oldState != _newState )
		wv::Application::getSingleton()->getInputSystem()->postActionEvent( this );
}