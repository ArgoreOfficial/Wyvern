#include "Time.h"

#ifdef WV_PLATFORM_WINDOWS
#include <chrono>
#endif

#include <thread>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Time::sleepForSeconds( double _seconds )
{
#ifdef WV_PLATFORM_WINDOWS
	std::chrono::milliseconds time( static_cast<std::chrono::milliseconds::rep>( _seconds * 1000 ) );
	std::this_thread::sleep_for( time );
#elif WV_PLATFORM_PSVITA
	xtime t;
	t.sec = _seconds;
	_Thrd_sleep( &t );
#elif WV_PLATFORM_3DS

#else
#error sleep_for not implemented on this platform
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
