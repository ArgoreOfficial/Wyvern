#pragma once

#include <arx/unordered_array.hpp>

#include <wv/Types.h>

#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>

#include "Worker.h"

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

	static wv::Job* createJob( wv::Fence* _pSignalFence, wv::Fence* _pWaitFence, wv::Job::JobFunction_t _fptr, void* _pData );
	void submit( const std::vector<wv::Job*>& _jobs );

	static Fence* createFence();
	static void   deleteFence( Fence* _fence );
	void waitForFences( wv::Fence** _pFences, size_t _count = 1 );

///////////////////////////////////////////////////////////////////////////////////////

	static void executeJob( wv::Job* _pJob );

protected:

	static void _workerThread( wv::JobSystem* _pJobSystem, wv::JobWorker* _pWorker );
	void _runWorker( wv::JobWorker* _pWorker );
	JobWorker* _getThisThreadWorker();

	std::vector<JobWorker*> m_workers;
	std::unordered_map<std::thread::id, JobWorker*> m_threadIDWorkerMap;

};

}