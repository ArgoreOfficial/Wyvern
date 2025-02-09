#include "JobBuffer.h"

#include "Job.h"
#include <wv/JobSystem/JobSystem.h>

#include <wv/Math/Math.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::JobBuffer::JobBuffer(  )
{
    m_jobs.resize( g_NUM_JOBS );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::JobBuffer::push( Job* _pJob )
{
	int bottom = m_bottom.load( std::memory_order_acquire );

	if ( bottom < static_cast<int>( m_jobs.size() ) )
	{
		m_jobs[ bottom ] = _pJob;
		m_bottom.store( bottom + 1, std::memory_order_release );

		return true;
	}

	return false; // queue is full
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::pop()
{
	long b = m_bottom.load( std::memory_order::acquire );
	b = wv::Math::max<size_t>( 0, b - 1 );
	m_bottom.store( b, std::memory_order::release );

	long t = m_top.load( std::memory_order::acquire );

	if ( t <= b )
	{
		Job* job = m_jobs[ b % g_NUM_JOBS ];
		if ( t != b )
			return job;
		
		if ( !m_top.compare_exchange_weak( t, t + 1, std::memory_order::acq_rel ) ) // failed race against steal operation
			job = nullptr;
		
		m_bottom.store( t + 1, std::memory_order::release );
		return job;
	}

	// deque was already empty
	m_bottom.store( t, std::memory_order::release );
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::steal()
{
	long t = m_top.load( std::memory_order::acquire ) ;
	std::atomic_signal_fence( std::memory_order_seq_cst );
	long b = m_bottom.load( std::memory_order::acquire );

	if ( t < b )
	{
		Job* job = m_jobs[ t % g_NUM_JOBS ];
		
		// a concurrent steal or pop operation removed an element from the deque in the meantime.
		if ( !m_top.compare_exchange_weak( t, t + 1, std::memory_order::acq_rel ) ) // failed race against steal operation
			return nullptr;

		return job;
	}

	return nullptr; // queue is empty
}
