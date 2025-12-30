#include "button_action.h"

#include <wv/debug/log.h>

#include <wv/application.h>
#include <wv/event/event_manager.h>

wv::ButtonAction::ButtonAction( const std::string& _name ) : IAction( _name ) 
{
	m_eventManager = wv::Application::getSingleton()->getEventManager();
}

uint32_t wv::ButtonAction::subscribeOnPressed( std::function<void( const ButtonOnPressedEvent& _event )> _callback )
{
	ActionEventListener<ButtonOnPressedEvent>* listener = WV_NEW( ActionEventListener<ButtonOnPressedEvent>, _callback );
	listener->setAction( this );
	m_eventManager->subscribeListener( listener );
	return m_onPressedListener.emplace( listener );
}

void wv::ButtonAction::unsubscribeOnPressed( uint32_t _listenerID )
{
	if ( !m_onPressedListener.contains( _listenerID ) )
		return;

	ActionEventListener<ButtonOnPressedEvent>* listener = m_onPressedListener.at( _listenerID );
	m_eventManager->unsubscribeListener( listener );

	WV_FREE( listener);
	m_onPressedListener.erase( _listenerID );
}

void wv::ButtonAction::updateState( bool _newState )
{
	bool oldState = m_state;
	m_state = _newState;
	
	if ( _newState && !oldState ) onPressed();
	if ( !_newState && oldState ) onReleased();
	if ( oldState != _newState ) onStateChanged();
}

void wv::ButtonAction::onPressed()
{
	ButtonOnPressedEvent event;
	event.actionID = m_actionID;
	m_eventManager->queueEvent( event );
}

void wv::ButtonAction::onReleased()
{
	ButtonOnReleasedEvent event;
	event.actionID = m_actionID;
	m_eventManager->queueEvent( event );
}

void wv::ButtonAction::onStateChanged()
{
	ButtonOnStateChangedEvent event;
	event.actionID = m_actionID;
	event.state = m_state;
	m_eventManager->queueEvent( event );
}
