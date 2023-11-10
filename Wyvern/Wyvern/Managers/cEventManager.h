#pragma once
#include <Wyvern/Core/iSingleton.h>
#include <Wyvern/Events/cEvents.h>
#include <functional>
#include <vector>

namespace WV
{
	class cEventManager : public iSingleton<cEventManager>
	{
	public:
		template<class T>
		static inline void hook( std::function<void( T& )> _func )
		{
			// static_assert( std::is_base_of<iEvent<T>, T>::value, "Invalid event class" );

			auto& hookedEvents = getHookedEvents<T>();
			hookedEvents.push_back( _func );
		}

		template<class T>
		static inline void call( T& _event )
		{
			// static_assert( std::is_base_of<iEvent<T>, T>::value, "Invalid event class" );

			auto& hookedEvents = getHookedEvents<T>();
			for ( int i = 0; i < hookedEvents.size(); i++ )
			{
				hookedEvents[ i ]( _event );
			}
		}

	private:
		template<class T>
		static inline std::vector<std::function<void( T& )>>& getHookedEvents()
		{
			static std::vector<std::function<void( T& )>> hookedEvents;
			return hookedEvents;
		}
	};
}