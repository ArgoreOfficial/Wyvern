#pragma once

#include <atomic>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

struct JobCounter
{
	std::atomic_int32_t value{ 0 };
};

///////////////////////////////////////////////////////////////////////////////////////

struct Job
{
	typedef void( *JobFunction_t )( const Job*, void* );

	JobFunction_t pFunction = nullptr;
	JobCounter** ppCounter = nullptr;

	char padding[ 44 ];
};


}