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
	int32_t bottom = m_bottom.load( std::memory_order_acquire );

	if ( bottom >= static_cast<int32_t>( m_jobs.size() ) )
		return false; // queue is full
	
	m_jobs[ bottom ] = _pJob;
	m_bottom.store( bottom + 1, std::memory_order_release );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::pop()
{
	int32_t b = m_bottom.load( std::memory_order::acquire );
	b = wv::Math::max<int32_t>( 0, b - 1 );
	m_bottom.store( b, std::memory_order::release );

	int32_t t = m_top.load( std::memory_order::acquire );

	if ( t > b )
	{
		// queue is empty
		m_bottom.store( t, std::memory_order::release );
		return nullptr;
	}

	Job* job = m_jobs[ b ];
	if ( t != b )
		return job;
		
	if ( !m_top.compare_exchange_weak( t, t + 1, std::memory_order::acq_rel ) )
		job = nullptr; // failed race against steal
		
	m_bottom.store( t + 1, std::memory_order::release );

	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobBuffer::steal()
{
	int32_t t = m_top.load( std::memory_order::acquire );
	int32_t b = m_bottom.load( std::memory_order::acquire );

	if ( t <= b )
		return nullptr; // queue is empty
	
	Job* job = m_jobs[ t % g_NUM_JOBS ];
		
	if ( !m_top.compare_exchange_weak( t, t + 1, std::memory_order::acq_rel ) )
		return nullptr; // failed race against steal or pop

	return job;
}
