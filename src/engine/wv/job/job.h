#pragma once

#include <wv/types.h>
#include <wv/decl.h>

#include <atomic>
#include <new>
#include <functional>

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

enum class JobThreadType
{
	kANY    = 0x0,
	kRENDER = 0x1
};

struct Job
{
	//typedef void( *JobFunction_t )( void* _pData );
	typedef std::function<void(void*)> JobFunction_t;

	JobThreadType threadType;
	JobFunction_t pFunction = nullptr;
	void* pData = nullptr;
	Fence* pSignalFence;

	//WV_PAD_PAYLOAD( 
	//	sizeof( void* )     +  // threadType // this is void* because of alignment
	//	sizeof( pFunction ) +
	//	sizeof( pData )     +
	//	sizeof( pSignalFence )
	//);
	//WV_PAD_TO_T( PAD_SIZE ) padding; // pad to PAD_SIZE bytes
};

//static_assert( sizeof( Job ) == PAD_SIZE );

}