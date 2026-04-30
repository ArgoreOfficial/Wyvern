#pragma once

#include <wv/reflection/reflection.h>

#include <functional>

namespace wv {

struct IEvent;

class IEventListener
{
	friend class EventManager;

public:
	IEventListener( TypeUUID _eventTypeUUID ) : 
		m_eventTypeUUID{ _eventTypeUUID } 
	{ }

	virtual ~IEventListener() { }

protected:
	virtual void triggerEvent( const IEvent& _event ) = 0;
	TypeUUID m_eventTypeUUID;
};

template<typename EventType>
class EventListener : public IEventListener
{
	friend class EventManager;

public:
	typedef std::function<void( const EventType& _event )> EventFunction_t;

	EventListener() :
		IEventListener( EventType::getStaticTypeUUID() )
	{ }

	EventListener( const EventFunction_t& _function ) : 
		IEventListener( EventType::getStaticTypeUUID() ),
		m_eventFunction{ _function }
	{ }

	virtual ~EventListener() 
	{ }

protected:
	virtual void triggerEvent( const IEvent& _event ) override {
		m_eventFunction( static_cast<const EventType&>( _event ) );
	}

	EventFunction_t m_eventFunction;
};

}

#define WV_FORWARD_EVENT(_function) [ = ]( const auto& _event ) { this->_function( _event ); }