#pragma once

#include <stdint.h>
#include <vector>

namespace wv
{
	struct sHook
	{
		bool hooked = false;
		int currentEventCounter = 0;
	};

	template<typename T>
	class cEventDispatcher
	{
	public:

		cEventDispatcher() { }

		static int32_t hook( int32_t _handle );
		static void unhook( int32_t _handle );

		static bool pollEvent( int32_t _handle, T& _event );

		static void post( T _event );
		static void flush();

	private:
		
		static inline std::vector<sHook> m_hooks {};
		static inline std::vector<T>     m_events{};

	};

	template<typename T>
	inline int32_t cEventDispatcher<T>::hook( int32_t _handle )
	{
		for ( size_t i = 0; i < m_hooks.size(); i++ )
		{
			if ( m_hooks[ i ].hooked )
				continue;

			_handle = i;
			break;
		}

		if ( _handle < 0 )
		{
			m_hooks.emplace_back( false, 0 );
			_handle = m_hooks.size() - 1;
		}

		m_hooks[ _handle ].hooked = true;
		m_hooks[ _handle ].currentEventCounter = m_events.size() - 1;

		return _handle;
	}

	template<typename T>
	inline void cEventDispatcher<T>::unhook( int32_t _handle )
	{
		if ( _handle < 0 || _handle >= m_hooks.size() )
			return;

		m_hooks[ _handle ].hooked = false;
		m_hooks[ _handle ].currentEventCounter = 0;
	}

	template<typename T>
	inline bool cEventDispatcher<T>::pollEvent( int32_t _handle, T& _outEvent )
	{
		if ( _handle < 0 || _handle >= m_hooks.size() )
			return false;

		int counter = m_hooks[ _handle ].currentEventCounter;
		if ( counter >= m_events.size() )
			return false;

		_outEvent = m_events[ counter ];
		m_hooks[ _handle ].currentEventCounter++;

		return true;
	}

	template<typename T>
	inline void cEventDispatcher<T>::post( T _event )
	{
		m_events.push_back( _event );
	}

	template<typename T>
	inline void cEventDispatcher<T>::flush()
	{
		m_events.clear();
		for ( auto& hook : m_hooks )
			hook.currentEventCounter = 0;
	}

}