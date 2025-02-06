#pragma once

#include <atomic>
#include <array>
#include <new>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

struct JobCounter
{
	std::atomic_int32_t value{ 0 };
};

#define DEF_PAD_PAYLOAD( _size ) static constexpr size_t _PAYLOAD_SIZE = _size ;
#define UCHAR_PAD_TO_T( _size ) static constexpr size_t _MAX_PADDING_SIZE = _size; \
static constexpr size_t _PADDING_SIZE = _MAX_PADDING_SIZE - _PAYLOAD_SIZE; \
std::array<unsigned char, _PADDING_SIZE>

///////////////////////////////////////////////////////////////////////////////////////

struct Job
{
	typedef void( *JobFunction_t )( const Job*, void* );

	JobFunction_t pFunction = nullptr;
	JobCounter** ppCounter = nullptr;
	void* pData = nullptr;

	DEF_PAD_PAYLOAD( sizeof( pFunction ) + sizeof( ppCounter ) + sizeof( pData ) );
	UCHAR_PAD_TO_T( std::hardware_destructive_interference_size ) padding;
};

static_assert( sizeof( Job ) == std::hardware_destructive_interference_size );


}