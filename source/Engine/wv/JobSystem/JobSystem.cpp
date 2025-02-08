#include "JobSystem.h"

#include <wv/Memory/Memory.h>
#include <exception>
#include <random>

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
	for ( size_t i = 0; i < m_workers.size(); i++ )
	{
		m_workers[ i ]->mIsAlive = false;
		
		if( m_workers[ i ]->mThread.joinable() )
			m_workers[ i ]->mThread.join();

		WV_FREE( m_workers[ i ] );
	}
	m_workers.clear();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::createJob( wv::Fence* _pSignalFence, wv::Fence* _pWaitFence, wv::Job::JobFunction_t _fptr, void* _pData )
{
	wv::Job* job = WV_NEW( wv::Job );
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

		worker->mQueue.push( j );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Fence* wv::JobSystem::createFence()
{
	wv::Fence* fence = WV_NEW( wv::Fence );
	fence->counter = 0;
	return fence;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::deleteFence( Fence* _fence )
{
	WV_FREE( _fence );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::waitForFences( wv::Fence** _pFences, size_t _count )
{
	for ( size_t i = 0; i < _count; i++ )
	{
		wv::Fence* fence = _pFences[ i ];
		while ( fence->counter )
		{
			JobWorker* worker = _getThisThreadWorker();
			Job* nextJob = _getNextJob( worker );
			if ( nextJob )
				wv::JobSystem::executeJob( nextJob );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_workerThread( wv::JobSystem* _pJobSystem, wv::JobWorker* _pWorker )
{
	while ( _pWorker->mIsAlive )
	{
		Job* nextJob = _pJobSystem->_getNextJob( _pWorker );
		if ( nextJob )
			wv::JobSystem::executeJob( nextJob );
	}
}

wv::JobWorker* wv::JobSystem::_getThisThreadWorker()
{
	std::thread::id threadID = std::this_thread::get_id();
	if ( m_threadIDWorkerMap.count( threadID ) )
		return m_threadIDWorkerMap[ threadID ];

	throw std::runtime_error( "Thread is not worker thread" );
	return nullptr; // will not occur
}

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

void wv::JobSystem::executeJob( wv::Job* _pJob )
{
	_pJob->pFunction( _pJob->pData );
	
	if ( _pJob->pSignalFence )
		_pJob->pSignalFence->counter--;

	// wv::JobSystem::_freeJob( _pJob );
	WV_FREE( _pJob );
}
