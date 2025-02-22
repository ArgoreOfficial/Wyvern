#pragma once

#include <wv/event/dispatcher.h>

namespace wv
{

	template<typename T>
	class EventListener
	{
	public:

		EventListener() { }


		void hook();
		void unhook();

		bool pollEvent( T& _outEvent );

	private:
		
		int32_t m_handle = -1;

	};

	template<typename T>
	inline void EventListener<T>::hook()
	{
		m_handle = EventDispatcher<T>::hook( m_handle );
	}

	template<typename T>
	inline void EventListener<T>::unhook()
	{
		EventDispatcher<T>::unhook( m_handle );
	}

	template<typename T>
	inline bool EventListener<T>::pollEvent( T& _outEvent )
	{
		return EventDispatcher<T>::pollEvent( m_handle, _outEvent );
	}

}