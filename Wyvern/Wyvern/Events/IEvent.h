#pragma once
#include <functional>
#include <Wyvern/Managers/EventManager.h>

namespace WV
{
	enum class EventType
	{

	};

	enum EventCategory
	{
		None = 0
	};

	template<class T>
	class IEvent
	{
	public:
		static inline void hook( std::function<void( T& )> _func ) { EventManager::hook<T>( _func ); }
		IEvent() { }

	protected:

	};
}
