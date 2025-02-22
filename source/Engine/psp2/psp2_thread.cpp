#ifdef WV_PLATFORM_PSP2

#include "PSP2Thread.h"

void wv::Thread::sleepFor( uint64_t _nanoseconds )
{
	xtime t;
	t.sec = _nanoseconds / 1000000000LL;
	_Thrd_sleep( &t );
}

#endif