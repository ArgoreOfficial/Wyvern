#include "JobSystem.h"

#include <wv/Memory/Memory.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::JobSystem::JobSystem()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::initialize( size_t _numWorkers )
{
	m_numWorkers = _numWorkers;
	createWorkers( _numWorkers );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::terminate()
{
	waitForAllJobs();
	
	killWorkers();

	deleteAll();
}

void wv::JobSystem::createWorkers( size_t _count )
{
	size_t count = _count;
	if ( _count == 0 )
		count = m_numWorkers;

	for ( size_t i = 0; i < count; i++ )
	{
		JobSystem::Worker* worker = WV_NEW( JobSystem::Worker );
		worker->thread = std::thread( _workerThread, this, worker );
		m_workers.push_back( worker );
	}
}

void wv::JobSystem::killWorkers()
{
	waitForAllJobs();

	for ( size_t i = 0; i < m_workers.size(); i++ )
	{
		m_workers[ i ]->alive = false;
		m_workers[ i ]->thread.join();
		WV_FREE( m_workers[ i ] );
	}
	m_workers.clear();
}

void wv::JobSystem::deleteAll()
{
	killWorkers();
	deleteAllJobs();
	deleteAllCounters();
}

void wv::JobSystem::deleteAllJobs()
{
	std::scoped_lock lock{ m_jobPoolMutex };

	for ( size_t i = 0; i < m_jobPool.size(); i++ )
		WV_FREE( m_jobPool[ i ] );

	m_jobPool.clear();

	while ( !m_availableJobs.empty() )
		m_availableJobs.pop();

}

void wv::JobSystem::deleteAllCounters()
{
	std::scoped_lock lock{ m_counterPoolMutex };
	
	for ( size_t i = 0; i < m_counterPool.size(); i++ )
		WV_FREE( m_counterPool[ i ] );
	m_counterPool.clear();

	while ( !m_availableCounters.empty() )
		m_availableCounters.pop();
}

void wv::JobSystem::waitForAllJobs()
{
	Job* nextJob = _getNextJob();
	while ( nextJob || !m_jobQueue.empty() )
	{
		_executeJob( nextJob );
		nextJob = _getNextJob();
	} 
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::createJob( const std::string& _name, Job::JobFunction_t _pFunction, JobCounter** _ppCounter, void* _pData )
{
	if ( _ppCounter && ( *_ppCounter ) == nullptr )
		*_ppCounter = _allocateCounter();

	Job* job = _allocateJob();

	job->name = _name;
	job->pFunction = _pFunction;
	job->pData = _pData;
	job->ppCounter = _ppCounter;

	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::run( Job** _pJobs, size_t _numJobs )
{
	for ( size_t i = 0; i < _numJobs; i++ )
	{
		Job* job = _pJobs[ i ];
		if ( job->ppCounter )
		{
			JobCounter* counter = *job->ppCounter;
			if ( counter )
				counter->value++;
		}
		
		m_queueMutex.lock();
		m_jobQueue.push_back( job );
		m_queueMutex.unlock();
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
		waitForCounter( _ppCounter, 0 );
	
	_freeCounter( *_ppCounter );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_workerThread( wv::JobSystem* _pJobSystem, wv::JobSystem::Worker* _pWorker )
{
	while ( _pWorker->alive )
	{
		Job* nextJob = _pJobSystem->_getNextJob();
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
	std::scoped_lock lock{ m_counterPoolMutex };

	wv::JobCounter* counter = nullptr;
	if ( m_availableCounters.empty() )
	{
		counter = WV_NEW( JobCounter );
		m_counterPool.push_back( counter );
	}
	else
	{
		counter = m_availableCounters.front();
		m_availableCounters.pop();
	}

	return counter;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::_allocateJob()
{
	std::scoped_lock lock{ m_jobPoolMutex };

	wv::Job* job = nullptr;
	if ( m_availableJobs.empty() )
	{
		job = WV_NEW( Job );
		m_jobPool.push_back( job );
	}
	else
	{
		job = m_availableJobs.front();
		m_availableJobs.pop();
	}

	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_freeCounter( JobCounter* _counter )
{
	m_counterPoolMutex.lock();
	_counter->value = 0;
	m_availableCounters.push( _counter );
	m_counterPoolMutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_freeJob( Job* _job )
{
	m_jobPoolMutex.lock();
	*_job = {};
	m_availableJobs.push( _job );
	m_jobPoolMutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_executeJob( Job* _job )
{
	_job->pFunction( _job, _job->pData );

	if ( _job->ppCounter )
	{
		JobCounter* counter = *_job->ppCounter;
		if ( counter )
			counter->value--;
	}

	_freeJob( _job );
}
