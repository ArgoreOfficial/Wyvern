#include "job_system.h"

#include <wv/memory/memory.h>
#include <wv/thread/thread.h>

void threadFunc( wv::TaskSystem* _jobSystem, wv::ThreadWorker* _worker )
{
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
			if ( m_tasks[ i ]->used.load() )
				WV_LOG_WARNING( "Unfinished job %i\n", (int)i );

			WV_FREE( m_tasks[ i ] );
		}
	}
}

void wv::ThreadWorker::push( const Task::Function& _task )
{
	std::scoped_lock lock{ m_mtx };

	const size_t index = m_tail % NUM_TASKS;

	if ( m_tasks[ index ] == nullptr )
		m_tasks[ index ] = WV_NEW( Task );

	m_tasks[ index ]->func = _task;
	m_tasks[ index ]->used.store( true );
	m_tail++;

	m_system->incrActiveTasks();
}

wv::Task* wv::ThreadWorker::pop()
{
	std::scoped_lock lock{ m_mtx };

	if ( m_tail - m_head <= 0 ) // empty
		return nullptr;

	m_tail--;
	Task* task = m_tasks[ m_tail % NUM_TASKS ];
	task->used.store( false );

	m_system->decrActiveTasks();

	return task;
}

wv::Task* wv::ThreadWorker::steal()
{
	std::scoped_lock lock{ m_mtx };

	if ( m_tail - m_head <= 0 ) // empty
		return nullptr;

	Task* task = m_tasks[ m_head % NUM_TASKS ];
	task->used.store( false );
	m_head++;

	m_system->decrActiveTasks();

	return task;
}

void wv::TaskSystem::createThreads( size_t _count )
{ 
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
}

void wv::TaskSystem::shutdownThreads()
{
	m_running.store( false );

	for ( auto& worker : m_workers )
		WV_FREE( worker );

	m_workers.clear();
}

void wv::TaskSystem::getAndExecute( ThreadWorker* _worker )
{
	if ( _worker == nullptr )
		return;

	if ( wv::Task* task = _worker->pop() )
		task->func();
	else if ( wv::ThreadWorker* otherWorker = getRandomThreadWorker() )
	{
		if ( wv::Task* task = getRandomThreadWorker()->steal() )
			task->func();
	}

	if ( numActiveTasks() > 0 )
		wv::Thread::yield();
	else
		wv::Thread::sleepForSeconds( 0.0001 );
}
