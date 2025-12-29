#pragma once

#include <wv/event/event_listener.h>
#include <wv/reflection/reflection.h>
#include <wv/memory/memory.h>

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
		static_assert( std::is_base_of<IEvent, Ty>(), "Event type must derive from IEvent" );

		EventPacket packet{};
		packet.eventTypeUUID = Ty::getStaticTypeUUID();
		packet.event = WV_NEW( Ty, _event );
		m_eventQueue.push_back( packet );
	}

	template<typename Ty>
	void triggerEvent( const Ty& _event );

	void subscribe( IEventListener* _listener );
	void unsubscribe( IEventListener* _listener );

	template<typename Ty>
	void subscribe( Ty* _listener, Ty::EventFunction_t _function ) {
		static_assert( std::is_base_of<IEventListener, Ty>(), "Must be an event listener" );

		_listener->m_eventFunction = _function;
		subscribe( static_cast<IEventListener*>( _listener ) );
	}

private:
	void processEvents();

	void triggerEventInternal( TypeUUID _eventTypeUUID, const IEvent& _event );

	bool m_isRunningEvent = false;
	std::vector<EventPacket> m_eventQueue;

	std::unordered_map<TypeUUID, std::vector<IEventListener*>> m_listeners;
};

template<typename Ty>
inline void EventManager::triggerEvent( const Ty& _event )
{
	static_assert( std::is_base_of<IEvent, Ty>(), "Event type must derive from IEvent" );
	WV_ASSERT_MSG( m_isRunningEvent == true, "Pushing an event inside a callback is not allowed (yet)" );
	
	m_isRunningEvent = true;
	triggerEventInternal( Ty::getStaticTypeUUID(), static_cast<const IEvent&>( _event ) );
	m_isRunningEvent = false;
}

}