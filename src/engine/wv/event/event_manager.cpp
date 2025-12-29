#include "event_manager.h"

wv::EventManager::~EventManager()
{
	// free up any left over events
	for ( auto event : m_eventQueue )
		WV_FREE( event.event );

	m_eventQueue.clear();
	m_listeners.clear();
}

void wv::EventManager::subscribe( IEventListener* _listener )
{
	if ( _listener == nullptr )
		return;

	if ( m_listeners.contains( _listener->m_eventTypeUUID ) )
	{
		// make sure listener is not already subscribed
		for ( auto listener : m_listeners[ _listener->m_eventTypeUUID ] )
		{
			if ( listener != _listener )
				continue;

			WV_LOG_ERROR( "Attempting to subscribe listener more than once!\n" );
			return;
		}
	}
	
	m_listeners[ _listener->m_eventTypeUUID ].push_back( _listener );
}

void wv::EventManager::unsubscribe( IEventListener* _listener )
{
	if ( _listener == nullptr )
		return;

	if ( !m_listeners.contains( _listener->m_eventTypeUUID ) )
		return;

	std::vector<IEventListener*>& vec = m_listeners[ _listener->m_eventTypeUUID ];
	
	// make sure listener is not already subscribed
	for ( auto it = vec.begin(); it != vec.end(); )
	{
		if ( *it != _listener ) continue;

		vec.erase( it );
		break;
	}
}

void wv::EventManager::processEvents()
{
	for ( auto event : m_eventQueue )
	{
		m_isRunningEvent = true;
		triggerEventInternal( event.eventTypeUUID, *event.event );
		m_isRunningEvent = false;

		WV_FREE( event.event ); // TODO: pool event copy allocations
	}

	m_eventQueue.clear();
}

void wv::EventManager::triggerEventInternal( TypeUUID _eventTypeUUID, const IEvent& _event )
{
	if ( !m_listeners.contains( _eventTypeUUID ) )
		return;

	for ( auto& listener : m_listeners.at( _eventTypeUUID ) )
		listener->triggerEvent( _event );
}
