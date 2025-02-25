#pragma once

#include <wv/unordered_array.hpp>

#include <wv/job/worker.h>
#include <wv/types.h>

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

	wv::Job* createJob( 
		wv::JobThreadType      _threadType, 
		wv::Job::JobFunction_t _fptr, 
		wv::Fence*             _pSignalFence = nullptr, 
		wv::Fence*             _pWaitFence   = nullptr, 
		void*                  _pData        = nullptr );

	wv::Job* createJob( 
		wv::Job::JobFunction_t _fptr, 
		wv::Fence*             _pSignalFence = nullptr, 
		wv::Fence*             _pWaitFence   = nullptr, 
		void*                  _pData        = nullptr );

	void submit( const std::vector<wv::Job*>& _jobs );

	Fence* createFence();
	void   deleteFence( Fence* _fence );

	void waitForFence( wv::Fence* _pFence );
	void waitAndDeleteFence( wv::Fence* _pFence );

	JobWorker* getThisThreadWorker();

///////////////////////////////////////////////////////////////////////////////////////

	void executeJob( wv::Job* _pJob );

protected:

	void _getNextAndExecuteJob( wv::JobWorker* _pWorker );

	static void _workerThread( wv::JobSystem* _pJobSystem, wv::JobWorker* _pWorker );
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