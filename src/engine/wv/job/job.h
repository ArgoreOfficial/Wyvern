#pragma once

#include <wv/types.h>
#include <wv/decl.h>

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

enum class JobThreadType : uint8_t
{
	kANY    = 0x0,
	kRENDER = 0x1
};

struct Job
{
	typedef void( *JobFunction_t )( void* _pData );

	JobThreadType threadType;
	JobFunction_t pFunction = nullptr;
	void* pData = nullptr;
	Fence* pSignalFence;

	unsigned char padding[ PAD_SIZE - 32 ]; // pad to PAD_SIZE bytes
};

static_assert( sizeof( Job ) == PAD_SIZE );

}