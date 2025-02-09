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
	int32_t head = m_head.load( std::memory_order::memory_order_acquire );

	if ( head >= static_cast<int32_t>( m_jobs.size() ) )
		return false; // queue is full
	
	m_jobs[ head ] = _pJob;
	m_head.store( head + 1, std::memory_order::memory_order_release );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::pop()
{
	int32_t head = m_head.load( std::memory_order::memory_order_acquire );
	head = wv::Math::max<int32_t>( 0, head - 1 );
	m_head.store( head, std::memory_order::memory_order_release );

	int32_t tail = m_tail.load( std::memory_order::memory_order_acquire );

	if ( tail > head )
	{
		// queue is empty
		m_head.store( tail, std::memory_order::memory_order_release );
		return nullptr;
	}

	Job* job = m_jobs[ head ];
	if ( tail != head )
		return job;
		
	if ( !m_tail.compare_exchange_weak( tail, tail + 1, std::memory_order::memory_order_acq_rel ) )
		job = nullptr; // failed race against steal
		
	m_head.store( tail + 1, std::memory_order::memory_order_release );

	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::steal()
{
	int32_t tail = m_tail.load( std::memory_order::memory_order_acquire );
	int32_t head = m_head.load( std::memory_order::memory_order_acquire );

	if ( tail <= head )
		return nullptr; // queue is empty
	
	Job* job = m_jobs[ tail % g_NUM_JOBS ];
		
	if ( !m_tail.compare_exchange_weak( tail, tail + 1, std::memory_order::memory_order_acq_rel ) )
		return nullptr; // failed race against steal or pop

	return job;
}
