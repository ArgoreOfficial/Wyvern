#pragma once

#include <wv/unordered_array.hpp>

#include <wv/JobSystem/Worker.h>
#include <wv/Types.h>

#include <vector>
#include <queue>

#include <thread>
#include <unordered_map>


///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

class JobSystem
{
public:

///////////////////////////////////////////////////////////////////////////////////////

	JobSystem();

	void initialize( size_t _numWorkers );
	void terminate();

	void createWorkers( size_t _count = 0 );
	void deleteWorkers();

	wv::Job* createJob( wv::Fence* _pSignalFence, wv::Fence* _pWaitFence, wv::Job::JobFunction_t _fptr, void* _pData );
	void submit( const std::vector<wv::Job*>& _jobs );

	Fence* createFence();
	void   deleteFence( Fence* _fence );

	void waitForFence( wv::Fence* _pFence );
	void waitAndDeleteFence( wv::Fence* _pFence );

///////////////////////////////////////////////////////////////////////////////////////

	void executeJob( wv::Job* _pJob );

protected:

	static void _workerThread( wv::JobSystem* _pJobSystem, wv::JobWorker* _pWorker );
	JobWorker* _getThisThreadWorker();
	Job* _getNextJob( wv::JobWorker* _pWorker );
	
	Job* _allocateJob();
	void _freeJob( Job* _pJob );
	
	std::mutex m_fencePoolMutex{};
	std::queue<Fence*> m_fencePool{};
	
	std::mutex m_jobPoolMutex{};
	std::queue<Job*> m_jobPool{};

	std::vector<JobWorker*> m_workers;
	std::unordered_map<std::thread::id, JobWorker*> m_threadIDWorkerMap;
};

}