#include "JobBuffer.h"

#include <wv/JobSystem/Job.h>
#include <wv/Math/Math.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::JobBuffer::JobBuffer(  )
{
    m_jobs.resize( g_NUM_JOBS );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::JobBuffer::push( Job* _pJob )
{
	int32_t h = m_head.load( std::memory_order::memory_order_acquire );
	
	m_jobs[ h % g_NUM_JOBS ] = _pJob;
	m_head.store( h + 1, std::memory_order::memory_order_release );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::pop()
{
	std::scoped_lock lock{ m_criticalMutex };

	int32_t h = m_head.load( std::memory_order::memory_order_acquire ) - 1;
	int32_t t = m_tail.load( std::memory_order::memory_order_acquire );

	if ( t <= h )
	{
		m_head.store( h, std::memory_order::memory_order_release );
		
		// non-empty queue
		Job* job = m_jobs[ h % g_NUM_JOBS ];
		if ( t != h )
        {
            // there's still more than one item left in the queue
            return job;
        }

		/*
		if ( !m_tail.compare_exchange_weak( t, t + 1, std::memory_order::memory_order_acq_rel ) )
		{
			// failed race against steal operation
			job = nullptr;
		}
		*/
		
		//m_head.store( t + 1, std::memory_order::memory_order_release );
		return job;
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::steal()
{
	std::scoped_lock lock{ m_criticalMutex };

	int32_t tail = m_tail.load( std::memory_order::memory_order_acquire );
	int32_t head = m_head.load( std::memory_order::memory_order_acquire );

	if ( tail < head )
	{
		Job* job = m_jobs[ tail % g_NUM_JOBS ];
		if ( !m_tail.compare_exchange_weak( tail, tail + 1, std::memory_order::memory_order_acq_rel ) )
			return nullptr; // failed race against steal or pop
		
		return job;
	}
	
	return nullptr; // queue is empty
}
