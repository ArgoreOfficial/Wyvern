#pragma once

#include <wv/Events/Dispatcher.h>

namespace wv
{

	template<typename T>
	class cEventListener
	{
	public:

		cEventListener() { }


		void hook();
		void unhook();

		bool pollEvent( T& _outEvent );

	private:
		
		int32_t m_handle = -1;

	};

	template<typename T>
	inline void cEventListener<T>::hook()
	{
		m_handle = cEventDispatcher<T>::hook( m_handle );
	}

	template<typename T>
	inline void cEventListener<T>::unhook()
	{
		cEventDispatcher<T>::unhook( m_handle );
	}

	template<typename T>
	inline bool cEventListener<T>::pollEvent( T& _outEvent )
	{
		return cEventDispatcher<T>::pollEvent( m_handle, _outEvent );
	}

}