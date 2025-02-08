#include "JobBuffer.h"

#include "Job.h"
#include <wv/JobSystem/JobSystem.h>

wv::JobBuffer::JobBuffer(  )
{
    m_jobs.resize( g_NUM_JOBS );
}

bool wv::JobBuffer::push( Job* _pJob )
{
	std::scoped_lock criticalLock{ m_criticalMutex };

	m_jobs[ m_head & g_MASK ] = _pJob;
	++m_head;

	return true;
}

wv::Job* wv::JobBuffer::pop()
{
	std::scoped_lock criticalLock{ m_criticalMutex };
	
	const int jobCount = m_head - m_tail;
	if ( jobCount <= 0 ) // no jobs queue
		return nullptr;
	
	--m_head;
	return m_jobs[ m_head & g_MASK ];
}

wv::Job* wv::JobBuffer::steal()
{
	std::scoped_lock criticalLock{ m_criticalMutex };

	const int jobCount = m_head - m_tail;
	if ( jobCount <= 0 ) // no job to steal
		return nullptr;
	
	Job* job = m_jobs[ m_tail & g_MASK ];
	++m_tail;
	return job;
}
