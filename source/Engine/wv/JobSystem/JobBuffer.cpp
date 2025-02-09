#include "JobBuffer.h"

#include "Job.h"
#include <wv/JobSystem/JobSystem.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::JobBuffer::JobBuffer(  )
{
    m_jobs.resize( g_NUM_JOBS );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobBuffer::push( Job* _pJob )
{
	size_t b = m_bottom;
	m_jobs[ b & g_MASK ] = _pJob;
	
	// ensure the job is written before b+1 is published to other threads.
	// on x86/64, a compiler barrier is enough.
	// On other platforms (PowerPC, ARM, …) you would need a memory fence instead
	// https://blog.molecular-matters.com/2015/09/25/job-system-2-0-lock-free-work-stealing-part-3-going-lock-free/
	std::atomic_signal_fence( std::memory_order_seq_cst );
	
	m_bottom = b + 1;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::pop()
{
	long b = m_bottom - 1;
	_InterlockedExchange( &m_bottom, b );
	
	long t = m_top;

	if ( t <= b )
	{
		Job* job = m_jobs[ b & g_MASK ];
		if ( t != b )
			return job;
		
		if ( _InterlockedCompareExchange( &m_top, t + 1, t ) != t ) // failed race against steal operation
			job = nullptr;
		
		m_bottom = t + 1;
		return job;
	}

	// deque was already empty
	m_bottom = t;
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::steal()
{
	long t = m_top;
	std::atomic_signal_fence( std::memory_order_seq_cst );
	long b = m_bottom;

	if ( t < b )
	{
		Job* job = m_jobs[ t & g_MASK ];

		// a concurrent steal or pop operation removed an element from the deque in the meantime.
		if ( _InterlockedCompareExchange( &m_top, t + 1, t ) != t )
			return nullptr;
		
		return job;
	}

	return nullptr; // queue empty
}
