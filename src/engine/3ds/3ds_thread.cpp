#ifdef WV_PLATFORM_3DS

#include "3ds_thread.h"

#include <3ds.h>

void wv::Thread::sleepFor( uint64_t _nanoseconds )
{
	svcSleepThread( _nanoseconds );
}


void wv::Thread::yield()
{
	// is there a svcYield?
	svcSleepThread( 0 );
}

#endif