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
	WV_PAD_TO_T( std::hardware_destructive_interference_size ) padding;
	//WV_PAD_TO_T( WV_CONCURRENCY ) padding;
};

static_assert( sizeof( Job ) == std::hardware_destructive_interference_size );


}