#include "job_system.h"

#include <wv/memory/memory.h>
#include <wv/thread/thread.h>

#include <tracy/Tracy.hpp>

void threadFunc( wv::TaskSystem* _jobSystem, wv::ThreadWorker* _worker )
{
	_jobSystem->waitForLock();

	while ( _jobSystem->isRunning() )
		_jobSystem->getAndExecute( _worker );
}

wv::ThreadWorker::~ThreadWorker()
{
	if( m_thread.joinable() )
		m_thread.join();

	for ( size_t i = 0; i < NUM_TASKS; i++ )
	{
		if ( m_tasks[ i ] != nullptr )
		{
			WV_FREE( m_tasks[ i ] );
		}
	}
}

void wv::ThreadWorker::push( Fence* _fence, const Task::Function& _task )
{
	std::scoped_lock lock{ m_mtx };

	if ( _fence )
		_fence->counter++;

	const size_t index = m_tail % NUM_TASKS;

	Task* task = m_system->allocateTask();
	task->func  = _task;
	task->fence = _fence;

	m_tasks[ index ] = task;
	m_tail++;

	m_system->incrActiveTasks();
}

wv::Task* wv::ThreadWorker::pop()
{
	std::scoped_lock lock{ m_mtx };

	if ( m_tail - m_head <= 0 ) // empty
		return nullptr;

	m_tail--;
	m_system->decrActiveTasks();
	return m_tasks[ m_tail % NUM_TASKS ];
}

wv::Task* wv::ThreadWorker::steal()
{
	std::scoped_lock lock{ m_mtx };

	if ( m_tail - m_head <= 0 ) // empty
		return nullptr;

	Task* task = m_tasks[ m_head % NUM_TASKS ];
	m_head++;

	m_system->decrActiveTasks();

	return task;
}

void wv::TaskSystem::createThreads( size_t _count )
{ 
	m_launchMutex.lock();
	m_running.store( true );

	// main thread worker, only used for pushing
	{
		ThreadWorker* mainThreadWorker = WV_NEW( ThreadWorker, this );
		m_threadWorkerMap.emplace( std::this_thread::get_id(), mainThreadWorker );
		m_workers.push_back( mainThreadWorker );
	}

	for ( size_t i = 0; i < _count; i++ )
	{
		ThreadWorker* worker = WV_NEW( ThreadWorker, this );
		worker->start( std::thread{ threadFunc, this, worker } );

		m_threadWorkerMap.emplace( worker->m_thread.get_id(), worker );
		m_workers.push_back( worker );
	}

	m_launchMutex.unlock();
}

void wv::TaskSystem::shutdownThreads()
{
	m_running.store( false );

	for ( auto& worker : m_workers )
		WV_FREE( worker );

	m_workers.clear();
	m_fencePool.clear();
}

void wv::TaskSystem::getAndExecute( ThreadWorker* _worker )
{
	if ( _worker == nullptr )
		return;

	wv::Task* task = _worker->pop();

	if ( task )
		executeTask( task );
	else 
	{
		// try to steal from main thread
		
		wv::ThreadWorker* otherWorker = getMainThreadWorker();
		if( otherWorker != _worker )
			task = otherWorker->steal();

		if ( !task )
		{
			otherWorker = getRandomThreadWorker();
			if ( otherWorker )
				task = otherWorker->steal();
		}

		if( task )
			executeTask( task );
	}
	
	if ( numActiveTasks() > 0 )
		wv::Thread::yield();
	else
		wv::Thread::sleepFor( 1000 );
}

void wv::TaskSystem::waitForFence( Fence* _fence )
{ 
	if ( _fence == nullptr )
		return;

	while ( _fence->counter.load() > 0 )
		getAndExecute( getThreadWorker() );
}

void wv::TaskSystem::executeTask( Task* _task )
{
	ZoneScoped;

	_task->func( this, _task->fence );
	if ( _task->fence )
		_task->fence->counter--;

	freeTask( _task );
}
