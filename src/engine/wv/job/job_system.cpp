#include "job_system.h"

#include <wv/memory/memory.h>
#include <exception>
#include <random>

#include <wv/debug/log.h>
#include <wv/debug/thread_profiler.h>
#include <wv/platform/thread.h>
#include <wv/engine.h>

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
	while( !m_fencePool.empty() )
	{
		std::scoped_lock lock{ m_fencePoolMutex };
		Fence* f = m_fencePool.front();
		m_fencePool.pop();

		waitForFence( f );
		WV_FREE( f );
	}

	deleteWorkers();

	m_jobPoolMutex.lock();
	while ( !m_jobPool.empty() )
	{
		Job* job = m_jobPool.front();
		m_jobPool.pop();
		
		WV_FREE( job );
	}
	m_jobPoolMutex.unlock();

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::createWorkers( size_t _count )
{
	m_workers.reserve( _count + 1 );
	ThreadProfiler* pProfiler = Engine::get()->m_pThreadProfiler;

	// host thread worker
	{
		JobWorker* worker = WV_NEW( JobWorker );
		worker->isRenderThread = true;
		worker->isHostThread   = true;
		worker->workTrace = pProfiler->getWorkTracer( std::this_thread::get_id() );
		
		m_workers.push_back( worker );
		m_threadIDWorkerMap[ std::this_thread::get_id() ] = worker;
	}

	for ( size_t i = 0; i < _count; i++ )
	{
		JobWorker* worker = WV_NEW( JobWorker );
		worker->thread = std::thread( _workerThread, this, worker );
		worker->workTrace = pProfiler->getWorkTracer( worker->thread.get_id() );

		m_workers.push_back( worker );
		m_threadIDWorkerMap[ worker->thread.get_id() ] = worker;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::deleteWorkers()
{
	// shutdown threads
	for ( size_t i = 1; i < m_workers.size(); i++ )
	{
		m_workers[ i ]->isAlive = false;
		m_workers[ i ]->thread.join();
	}

	for ( size_t i = 0; i < m_workers.size(); i++ )
		WV_FREE( m_workers[ i ] );
	
	m_workers.clear();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::createJob( wv::Job::JobFunction_t _fptr, wv::Fence* _pSignalFence, wv::Fence* _pWaitFence, void* _pData )
{
	return createJob( JobThreadType::kANY, _fptr, _pSignalFence, _pWaitFence, _pData );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::createJob( wv::JobThreadType _threadType, wv::Job::JobFunction_t _fptr, wv::Fence* _pSignalFence, wv::Fence* _pWaitFence, void* _pData )
{
	Job* job = _allocateJob();
	job->pSignalFence = _pSignalFence;
	job->pFunction = _fptr;
	job->pData = _pData;
	job->threadType = _threadType;
	return job;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::submit( const std::vector<wv::Job*>& _jobs )
{
	JobWorker* worker = getThisThreadWorker();
	for ( auto& j : _jobs )
	{
		if ( j->pSignalFence )
			j->pSignalFence->counter++;

		if ( !worker->queue.push( j ) )
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
		JobWorker* worker = getThisThreadWorker();
		_getNextAndExecuteJob( worker );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::waitAndDeleteFence( wv::Fence* _pFence )
{
	waitForFence( _pFence );
	deleteFence( _pFence );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::JobWorker* wv::JobSystem::getThisThreadWorker()
{
	std::thread::id threadID = std::this_thread::get_id();
	if( m_threadIDWorkerMap.count( threadID ) )
		return m_threadIDWorkerMap[ threadID ];
#ifdef __cpp_exceptions
	throw std::runtime_error( "Thread is not worker thread" );
#else
	wv::Debug::Print( wv::Debug::WV_PRINT_FATAL, "Thread is not worker thread\n" );
#endif
	return nullptr; // will not occur
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_getNextAndExecuteJob( wv::JobWorker* _pWorker )
{
	Job* nextJob = _getNextJob( _pWorker );
	if( nextJob )
	{
		JobThreadType req = nextJob->threadType;
		
		if( req == JobThreadType::kANY || ( req == JobThreadType::kRENDER && _pWorker->isRenderThread ) )
		{
			wv::ThreadWorkTrace::StackFrame frame = _pWorker->workTrace->begin();
			wv::JobSystem::executeJob( nextJob );
			_pWorker->workTrace->end( frame );
		}
		else
			_pWorker->queue.push( nextJob );
	}
	else 
		if ( !_pWorker->isHostThread ) 
			wv::Thread::sleepFor( 10 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::JobSystem::_workerThread( wv::JobSystem* _pJobSystem, wv::JobWorker* _pWorker )
{
	while ( _pWorker->isAlive )
		_pJobSystem->_getNextAndExecuteJob( _pWorker );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::_getNextJob( wv::JobWorker* _pWorker )
{
	Job* job = _pWorker->queue.pop();

	if ( job )
		return job;

	const int r = std::rand() % m_workers.size();
	JobWorker* worker = m_workers[ r ];

	if ( worker == _pWorker )
		return nullptr;
	
	Job* stolenJob = worker->queue.steal();

	if ( stolenJob )
		return stolenJob;
	
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Job* wv::JobSystem::_allocateJob()
{
	wv::Job* job = nullptr;
	
	std::scoped_lock lock{ m_jobPoolMutex };
	if ( m_jobPool.empty() )
		job = WV_NEW( Job );
	else
	{
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
