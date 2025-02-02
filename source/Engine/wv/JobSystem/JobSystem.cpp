#include "JobSystem.h"

#include <wv/Memory/Memory.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::JobSystem::JobSystem()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::initialize( size_t _numWorkers )
{
	for ( int i = 0; i < _numWorkers; i++ )
	{
		JobSystem::Worker* worker = WV_NEW( JobSystem::Worker );
		worker->thread = std::thread( _workerThread, this, worker );
		m_workers.push_back( worker );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::terminate()
{
	for ( size_t i = 0; i < m_workers.size(); i++ )
	{
		m_workers[ i ]->alive = false;
		m_workers[ i ]->thread.join();
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::JobSystem::JobID wv::JobSystem::createJob( const std::string& _name, Job::JobFunction_t _pFunction, void* _pData )
{

	Job job{};
	job.name = _name;
	job.pFunction = _pFunction;
	job.pData = _pData;
	job.ppCounter = nullptr;

	m_queueMutex.lock();
	JobID id = m_jobPool.emplace( job );
	m_queueMutex.unlock();
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::run( JobID* _pJobs, size_t _numJobs, JobCounter** _ppCounter )
{
	if ( _ppCounter && (*_ppCounter) == nullptr )
		*_ppCounter = _allocateCounter();
	

	for ( size_t i = 0; i < _numJobs; i++ )
	{
		m_queueMutex.lock();
		Job& job = m_jobPool.at( _pJobs[ i ] );
		job.ppCounter = _ppCounter;

		m_jobQueue.push_back( _pJobs[ i ] );
		m_queueMutex.unlock();
		
		if ( _ppCounter && *_ppCounter )
			( *_ppCounter )->value++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::waitForCounter( JobCounter** _ppCounter, int _value )
{
	if ( !_ppCounter || !*_ppCounter )
	{
		// warning: nullptr
		return;
	}

	JobCounter& counter = **_ppCounter;
	while ( counter.value != _value )
	{
		JobID nextJob = _getNextJob();
		if ( nextJob.is_valid() )
			_executeJob( nextJob );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::waitForAndFreeCounter( JobCounter** _ppCounter, int _value )
{
	waitForCounter( _ppCounter, _value );
	freeCounter( _ppCounter );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::freeCounter( JobCounter** _ppCounter )
{
	if ( !_ppCounter || !*_ppCounter )
	{
		// warning: nullptr
		return;
	}

	JobCounter& counter = **_ppCounter;
	if ( counter.value > 0 )
		waitForCounter( _ppCounter, 0 );
	
	WV_FREE( *_ppCounter );
	*_ppCounter = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_workerThread( wv::JobSystem* _pJobSystem, wv::JobSystem::Worker* _pWorker )
{
	while ( _pWorker->alive )
	{
		JobID nextJob = _pJobSystem->_getNextJob();
		if ( nextJob.is_valid() )
			_pJobSystem->_executeJob( nextJob );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::JobSystem::JobID wv::JobSystem::_getNextJob()
{
	std::scoped_lock lock{ m_queueMutex };

	if ( m_jobQueue.empty() )
		return JobID::InvalidID;

	JobID job = m_jobQueue.back();
	m_jobQueue.pop_back();
	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::JobCounter* wv::JobSystem::_allocateCounter()
{
	JobCounter* newCounter = WV_NEW( JobCounter );
	newCounter->value = 0;
	return newCounter;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_executeJob( JobID _job )
{
	m_queueMutex.lock();
	Job job = m_jobPool.at( _job );
	m_queueMutex.unlock();

	job.pFunction( job, job.pData );

	if ( job.ppCounter && *job.ppCounter )
	{
		JobCounter* counter = *job.ppCounter;
		counter->value--;
	}

	m_queueMutex.lock();
	m_jobPool.erase( _job );
	m_queueMutex.unlock();
}
