#include "event_manager.h"

wv::EventManager::~EventManager()
{
	// free up any left over events
	for ( auto event : m_eventQueue )
		WV_FREE( event.event );

	// free up any left over listeners
	for ( auto key : m_allocatedListeners.keys() )
	{
		IEventListener* listener = m_allocatedListeners.at( key );;
		if ( listener )
			WV_FREE( listener );
	}

	m_allocatedListeners.clear();
	m_eventQueue.clear();
	m_subscribedListeners.clear();
}

void wv::EventManager::subscribeListener( IEventListener* _listener )
{
	if ( _listener == nullptr )
		return;

	if ( m_subscribedListeners.contains( _listener->m_eventTypeUUID ) )
	{
		// make sure listener is not already subscribed
		for ( auto listener : m_subscribedListeners[ _listener->m_eventTypeUUID ] )
		{
			if ( listener != _listener )
				continue;

			WV_LOG_ERROR( "Attempting to subscribe listener more than once!\n" );
			return;
		}
	}
	
	m_subscribedListeners[ _listener->m_eventTypeUUID ].push_back( _listener );
}

void wv::EventManager::unsubscribeListener( IEventListener* _listener )
{
	if ( _listener == nullptr )
		return;

	if ( !m_subscribedListeners.contains( _listener->m_eventTypeUUID ) )
		return;

	std::vector<IEventListener*>& vec = m_subscribedListeners[ _listener->m_eventTypeUUID ];
	
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
	for ( size_t i = 0; i < m_eventQueue.size(); i++ )
	{
		m_isRunningEvent = true;
		triggerEventInternal( m_eventQueue[ i ].eventTypeUUID, *m_eventQueue[ i ].event);
		m_isRunningEvent = false;

		WV_FREE( m_eventQueue[ i ].event ); // TODO: pool event copy allocations
	}

	m_eventQueue.clear();
}

void wv::EventManager::triggerEventInternal( TypeUUID _eventTypeUUID, const IEvent& _event )
{
	if ( !m_subscribedListeners.contains( _eventTypeUUID ) )
		return;

	for ( auto& listener : m_subscribedListeners.at( _eventTypeUUID ) )
		listener->triggerEvent( _event );
}
