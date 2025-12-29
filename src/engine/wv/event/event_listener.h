#pragma once

#include <wv/reflection/reflection.h>

#include <functional>

namespace wv {

struct IEvent;

class IEventListener : public IReflectedType
{
	friend class EventManager;

	WV_REFLECT_TYPE( IEventListener )
public:
	IEventListener( TypeUUID _eventTypeUUID ) : 
		m_eventTypeUUID{ _eventTypeUUID } 
	{ }

protected:
	virtual void triggerEvent( const IEvent& _event ) = 0;
	TypeUUID m_eventTypeUUID;
};

template<typename EventType>
class EventListener : public IEventListener
{
public:
	typedef std::function<void( const EventType& _event )> EventFunction_t;

	EventListener( const EventFunction_t& _function ) : 
		IEventListener( EventType::getStaticTypeUUID() ),
		m_eventFunction{ _function }
	{ }

protected:
	virtual void triggerEvent( const IEvent& _event ) {
		m_eventFunction( static_cast<const EventType&>( _event ) );
	}

private:
	EventFunction_t m_eventFunction;
};

}