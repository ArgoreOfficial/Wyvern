#include "JobSystem.h"

#include <wv/Memory/Memory.h>
#include <exception>
#include <random>

#include <wv/Debug/Print.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::JobSystem::JobSystem()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::initialize( size_t _numWorkers )
{
	createWorkers( _numWorkers );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::terminate()
{
	while ( !m_jobPool.empty() )
	{
		WV_FREE( m_jobPool.front() );
		m_jobPool.pop();
	}

	while ( !m_fencePool.empty() )
	{
		std::scoped_lock lock{ m_fencePoolMutex };
		Fence* f = m_fencePool.front();
		m_fencePool.pop();

		waitForFence( f );
		WV_FREE( f );
	}

	deleteWorkers();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::createWorkers( size_t _count )
{
	m_workers.reserve( _count + 1 );
	
	// main thread worker
	{
		JobWorker* worker = WV_NEW( JobWorker );
		m_workers.push_back( worker );
		m_threadIDWorkerMap[ std::this_thread::get_id() ] = worker;
	}

	for ( size_t i = 0; i < _count; i++ )
	{
		JobWorker* worker = WV_NEW( JobWorker );
		worker->mThread = std::thread( _workerThread, this, worker );

		m_workers.push_back( worker );
		m_threadIDWorkerMap[ worker->mThread.get_id() ] = worker;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::deleteWorkers()
{
	// shutdown threads
	for ( size_t i = 1; i < m_workers.size(); i++ )
	{
		m_workers[ i ]->mIsAlive = false;
		m_workers[ i ]->mThread.join();
	}

	for ( size_t i = 0; i < m_workers.size(); i++ )
		WV_FREE( m_workers[ i ] );
	
	m_workers.clear();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::createJob( wv::Fence* _pSignalFence, wv::Fence* _pWaitFence, wv::Job::JobFunction_t _fptr, void* _pData )
{
	Job* job = _allocateJob();
	job->pSignalFence = _pSignalFence;
	job->pFunction    = _fptr;
	job->pData        = _pData;
	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::submit( const std::vector<wv::Job*>& _jobs )
{
	JobWorker* worker = _getThisThreadWorker();
	for ( auto& j : _jobs )
	{
		if ( j->pSignalFence )
			j->pSignalFence->counter++;

		if ( !worker->mQueue.push( j ) )
			JobSystem::executeJob( j );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Fence* wv::JobSystem::createFence()
{
	wv::Fence* fence = nullptr;

	if ( m_fencePool.empty() )
		fence = WV_NEW( wv::Fence );
	else
	{
		std::scoped_lock lock{ m_fencePoolMutex };
		fence = m_fencePool.front();
		m_fencePool.pop();
	}

	fence->counter = 0;

	return fence;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::deleteFence( Fence* _fence )
{
	std::scoped_lock lock{ m_fencePoolMutex };
	m_fencePool.push( _fence );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::waitForFence( wv::Fence* _pFence )
{
	while ( _pFence->counter )
	{
		JobWorker* worker = _getThisThreadWorker();
		Job* nextJob = _getNextJob( worker );
		if ( nextJob )
			wv::JobSystem::executeJob( nextJob );
	}
}

void wv::JobSystem::waitAndDeleteFence( wv::Fence* _pFence )
{
	waitForFence( _pFence );
	deleteFence( _pFence );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_workerThread( wv::JobSystem* _pJobSystem, wv::JobWorker* _pWorker )
{
	while ( _pWorker->mIsAlive )
	{
		Job* nextJob = _pJobSystem->_getNextJob( _pWorker );
		if ( nextJob )
			_pJobSystem->executeJob( nextJob );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::JobWorker* wv::JobSystem::_getThisThreadWorker()
{
	std::thread::id threadID = std::this_thread::get_id();
	if ( m_threadIDWorkerMap.count( threadID ) )
		return m_threadIDWorkerMap[ threadID ];
#ifdef __cpp_exceptions
	throw std::runtime_error( "Thread is not worker thread" );
#else
	wv::Debug::Print( wv::Debug::WV_PRINT_FATAL, "Thread is not worker thread\n" );
#endif
	return nullptr; // will not occur
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::_getNextJob( wv::JobWorker* _pWorker )
{
	Job* job = _pWorker->mQueue.pop();

	if ( job )
		return job;

	const int r = std::rand() % m_workers.size();
	JobWorker* worker = m_workers[ r ];

	if ( worker == _pWorker )
	{
		std::this_thread::yield();
		return nullptr;
	}

	Job* stolenJob = worker->mQueue.steal();

	if ( stolenJob )
		return stolenJob;
		
	std::this_thread::yield();
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::_allocateJob()
{
	wv::Job* job = nullptr;
	
	if ( m_jobPool.empty() )
		job = WV_NEW( Job );
	else
	{
		std::scoped_lock lock{ m_jobPoolMutex };
		job = m_jobPool.front();
		m_jobPool.pop();
	}

	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_freeJob( Job* _pJob )
{
	std::scoped_lock lock{ m_jobPoolMutex };
	m_jobPool.push( _pJob );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::executeJob( wv::Job* _pJob )
{
	_pJob->pFunction( _pJob->pData );
	
	if ( _pJob->pSignalFence )
		_pJob->pSignalFence->counter--;

	_freeJob( _pJob );
}
