#pragma once

#include <wv/math/math.h>
#include <wv/memory/memory.h>

#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <queue>

namespace wv {

class TaskSystem;

static inline constexpr size_t NUM_TASKS = 2048;

template<typename Ty>
class Pool
{
public:
	Ty* allocate() {
		Ty* ptr = nullptr;

		std::scoped_lock lock{ m_mtx };

		if ( m_pool.empty() )
			ptr = WV_NEW( Ty );
		else
		{
			ptr = m_pool.front();
			m_pool.pop();
		}

		return ptr;
	}

	void free( Ty* _ptr ) {
		std::scoped_lock lock{ m_mtx };
		m_pool.push( _ptr );
	}

	void clear() {
		std::scoped_lock lock{ m_mtx };

		while ( !m_pool.empty() )
		{
			Ty* ptr = m_pool.front();
			m_pool.pop();
			WV_FREE( ptr );
		}
	}

private:
	std::mutex m_mtx;
	std::queue<Ty*> m_pool{};
};

struct Fence
{
	std::atomic_uint32_t counter;
};

struct Task
{
	typedef std::function<void( TaskSystem* _system, Fence* _fence )> Function;

	Function func;
	Fence* fence = nullptr;
};

class ThreadWorker
{
	friend class TaskSystem;

public:
	ThreadWorker( TaskSystem* _system ) : m_system{ _system } { }
	~ThreadWorker();

	void start( std::thread&& _thread ) {
		m_thread = std::move( _thread );
	}
	
	void push( Fence* _fence, const Task::Function& _task );
	void push( const Task::Function& _task ) { push( nullptr, _task ); }

	Task* pop();
	Task* steal();

private:
	TaskSystem* m_system = nullptr;
	std::thread m_thread;

	uint32_t m_head = 0;
	uint32_t m_tail = 0;
	Task* m_tasks[ NUM_TASKS ] = { nullptr };
	std::mutex m_mtx{};
};

class TaskSystem
{
public:
	TaskSystem() = default;
	
	void createThreads( size_t _count );
	void shutdownThreads();

	bool isRunning()  const { return m_running.load(); }
	
	uint32_t numActiveTasks() { return m_activeTasks.load(); }

	void incrActiveTasks() { m_activeTasks++; }
	void decrActiveTasks() { m_activeTasks--; }

	ThreadWorker* getRandomThreadWorker() {
		return m_workers[ wv::Math::randomU32() % m_workers.size() ];
	}

	ThreadWorker* getThreadWorker() {
		std::thread::id id = std::this_thread::get_id();

		if ( !m_threadWorkerMap.contains( id ) )
			return nullptr; // shouldn't occur

		return m_threadWorkerMap.at( id );
	}

	void getAndExecute( ThreadWorker* _worker );

	Fence* allocateFence() { return m_fencePool.allocate(); }
	void freeFence( Fence* _fence ) { m_fencePool.free( _fence ); }
	
	void waitForFence( Fence* _fence );

	void waitAndFreeFence( Fence* _fence ) {
		waitForFence( _fence );
		freeFence( _fence );
	}

private:
	std::atomic_uint32_t m_activeTasks = 0;

	std::atomic_bool m_running = false;
	std::vector<ThreadWorker*> m_workers = {};
	std::unordered_map<std::thread::id, ThreadWorker*> m_threadWorkerMap = {};
	Pool<Fence> m_fencePool;
};

}