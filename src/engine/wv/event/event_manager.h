#pragma once

#include <wv/event/event_listener.h>
#include <wv/reflection/reflection.h>
#include <wv/memory/memory.h>
#include <wv/helpers/unordered_array.hpp>

#include <vector>
#include <unordered_map>

namespace wv {

struct EventPacket
{
	TypeUUID eventTypeUUID;
	IEvent* event;
};

class EventManager
{
public:
	friend class Application;

	~EventManager();

	template<typename Ty>
	void queueEvent( const Ty& _event )
	{
		static_assert( std::is_base_of<IEvent, Ty>(), "Must be a valid IEvent type" );

		EventPacket packet{};
		packet.eventTypeUUID = Ty::getStaticTypeUUID();
		packet.event = WV_NEW( Ty, _event );
		m_eventQueue.push_back( packet );
	}

	template<typename Ty>
	void triggerEvent( const Ty& _event );

	void subscribeListener( IEventListener* _listener );
	void unsubscribeListener( IEventListener* _listener );

	template<typename Ty>
	uint32_t subscribe( std::function<void( const Ty& _event )> _callback )
	{
		static_assert( std::is_base_of<IEvent, Ty>(), "Must be a valid IEvent type" );

		EventListener<Ty>* listener = WV_NEW( EventListener<Ty>, _callback );
		subscribeListener( static_cast<IEventListener*>( listener ) );

		return m_allocatedListeners.emplace( static_cast<IEventListener*>( listener ) );
	}

	void unsubscribe( uint32_t _listenerID ) {
		if ( !m_allocatedListeners.contains( _listenerID ) ) return;
		
		WV_FREE( m_allocatedListeners.at( _listenerID ) );
		m_allocatedListeners.erase( _listenerID );
	}

private:
	void processEvents();
	void triggerEventInternal( TypeUUID _eventTypeUUID, const IEvent& _event );

	bool m_isRunningEvent = false;
	std::vector<EventPacket> m_eventQueue;

	// Listeners allocated with subscribe<Ty>()
	wv::unordered_array<uint32_t, IEventListener*> m_allocatedListeners;
	
	// All currently subscribed listeners. 
	// May include externally implemented and allocated listeners
	std::unordered_map<TypeUUID, std::vector<IEventListener*>> m_subscribedListeners;
};

template<typename Ty>
inline void EventManager::triggerEvent( const Ty& _event )
{
	static_assert( std::is_base_of<IEvent, Ty>(), "Event type must derive from IEvent" );
	WV_ASSERT_MSG( m_isRunningEvent != true, "Pushing an event inside a callback is not allowed (yet)" );
	
	m_isRunningEvent = true;
	triggerEventInternal( Ty::getStaticTypeUUID(), static_cast<const IEvent&>( _event ) );
	m_isRunningEvent = false;
}

}