#include "JobBuffer.h"

#include "Job.h"

wv::JobBuffer::JobBuffer( size_t _size ) :
    m_numJobs{ _size }
{
    m_jobs.resize( _size );
}

void wv::JobBuffer::push( Job* _pJob )
{
    size_t idx = (m_head - 1) % m_numJobs;
    Job*& job = *m_jobs[ idx ].lock();

    if( job == nullptr )
        job = _pJob;
    else
    {
        job->pFunction( job, job->pData );
        job = nullptr;
        m_head--;
    }

    m_jobs[ idx ].unlock();
    
    m_head++;
    m_head %= m_jobs.size();
}

wv::Job* wv::JobBuffer::pop()
{
    Job*& job = *m_jobs[ m_head ].lock();

    Job* jobCopy = job;
    job = nullptr;

    m_jobs[ m_head ].unlock();

    if( jobCopy != nullptr )
        m_head--;
    m_head %= m_numJobs;

    return jobCopy;
}

wv::Job* wv::JobBuffer::steal()
{
    uint32_t& tail = *m_tail.lock();
    Job** pJob;
    if( !m_jobs[ tail ].try_lock( pJob ) )
    {
        m_tail.unlock();
        return nullptr;
    }

    Job*& job = *pJob;
    Job* jobCopy = job;
    job = nullptr;

    m_jobs[ tail ].unlock();

    if( jobCopy )
        tail--;
    tail %= m_numJobs;
    m_tail.unlock();

    return jobCopy;
}
