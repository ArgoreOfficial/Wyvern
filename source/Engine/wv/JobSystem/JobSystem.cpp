#include "JobSystem.h"

#include <atomic>

///////////////////////////////////////////////////////////////////////////////////////

wv::JobSystem::JobSystem()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::initialize( size_t _numWorkers )
{
	for ( int i = 0; i < _numWorkers; i++ )
	{
		JobSystem::Worker* worker = new JobSystem::Worker();
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

wv::Job* wv::JobSystem::createJob( const std::string& _name, Job::JobFunction_t _pFunction, void* _pData )
{
	Job* job = new Job();
	job->name = _name;
	job->pFunction = _pFunction;
	job->pData = _pData;
	job->ppCounter = nullptr;
	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::run( Job** _ppJobs, size_t _numJobs, JobCounter** _ppCounter )
{
	if ( _ppCounter && (*_ppCounter) == nullptr )
		*_ppCounter = _allocateCounter();
	

	for ( size_t i = 0; i < _numJobs; i++ )
	{
		std::scoped_lock lock{ m_queueMutex };

		_ppJobs[ i ]->ppCounter = _ppCounter;
		m_jobQueue.push_back( _ppJobs[ i ] );
		
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
		Job* nextJob = _getNextJob();
		if ( nextJob )
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
	{
		// warning: counter has jobs running
		return;
	}

	delete *_ppCounter;
	*_ppCounter = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_workerThread( wv::JobSystem* _pJobSystem, wv::JobSystem::Worker* _pWorker )
{
	while ( _pWorker->alive )
	{
		wv::Job* nextJob = _pJobSystem->_getNextJob();
		if ( nextJob )
			_pJobSystem->_executeJob( nextJob );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::_getNextJob()
{
	std::scoped_lock lock{ m_queueMutex };

	if ( m_jobQueue.empty() )
		return nullptr;

	Job* job = m_jobQueue.back();
	m_jobQueue.pop_back();
	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::JobCounter* wv::JobSystem::_allocateCounter()
{
	JobCounter* newCounter = new JobCounter();
	newCounter->value = 0;
	return newCounter;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_executeJob( Job* _pJob )
{
	_pJob->pFunction( _pJob, _pJob->pData );

	if ( _pJob->ppCounter && *_pJob->ppCounter )
		( *_pJob->ppCounter )->value--;
	
	delete _pJob;
}
