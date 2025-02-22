#ifdef WV_PLATFORM_WINDOWS

#include "WinThread.h"

#include <chrono>

void wv::Thread::sleepFor( uint64_t _nanoseconds )
{
	std::chrono::nanoseconds time{ _nanoseconds };
	std::this_thread::sleep_for( time );
}

#endif