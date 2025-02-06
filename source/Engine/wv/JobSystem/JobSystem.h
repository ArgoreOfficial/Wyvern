#pragma once

#include <arx/unordered_array.hpp>

#include <wv/Types.h>

#include <deque>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <atomic>

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
	void killWorkers();

	void deleteAll();
	void deleteAllJobs();
	void deleteAllCounters();

	void waitForAllJobs();

	[[nodiscard]] Job* createJob( const std::string& _name, Job::JobFunction_t _pFunction, JobCounter** _ppCounter = nullptr, void* _pData = nullptr );
	[[nodiscard]] Job* createJob( Job::JobFunction_t _pFunction, JobCounter** _ppCounter = nullptr, void* _pData = nullptr )
	{
		return createJob( "", _pFunction, _ppCounter, _pData );
	}

	void run( Job** _pJobs, size_t _numJobs = 1 );
	void waitForCounter( JobCounter** _ppCounter, int _value );
	void waitForAndFreeCounter( JobCounter** _ppCounter, int _value );

	void freeCounter( JobCounter** _ppCounter );

///////////////////////////////////////////////////////////////////////////////////////

protected:

	static void _workerThread( wv::JobSystem* _pJobSystem, wv::JobWorker* _pWorker );

	Job* _getNextJob();
	
	JobCounter* _allocateCounter();
	Job* _allocateJob();

	void _freeCounter( JobCounter* _counter );
	void _freeJob( Job* _job );

	void _executeJob( Job* _job );

	std::mutex m_queueMutex{};
	std::mutex m_jobPoolMutex{};
	std::mutex m_counterPoolMutex{};

	std::vector<JobCounter*> m_counterPool{};
	std::queue <JobCounter*> m_availableCounters{};

	std::vector<Job*> m_jobPool{};
	std::queue <Job*> m_availableJobs{};
	std::deque <Job*> m_jobQueue{};

	std::vector<JobWorker*> m_workers;
	size_t m_numWorkers = 0;

};

}