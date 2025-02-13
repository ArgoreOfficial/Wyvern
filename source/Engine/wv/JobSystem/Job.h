#pragma once

#include <wv/Types.h>
#include <wv/Decl.h>

#include <atomic>
#include <new>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

struct Fence
{
	std::atomic_uint32_t counter;
};

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_PLATFORM_WINDOWS 
#define PAD_SIZE std::hardware_destructive_interference_size
#else
#define PAD_SIZE 32
#endif

struct Job
{
	typedef void( *JobFunction_t )( void* _pData );

	JobFunction_t pFunction = nullptr;
	void* pData = nullptr;
	Fence* pSignalFence;

	// pad to 64 bytes
	WV_PAD_PAYLOAD( 
		sizeof( pFunction ) + 
		sizeof( pData ) +
		sizeof( pSignalFence )
	);
	WV_PAD_TO_T( PAD_SIZE ) padding;
	//WV_PAD_TO_T( WV_CONCURRENCY ) padding;
};

static_assert( sizeof( Job ) == PAD_SIZE );


}