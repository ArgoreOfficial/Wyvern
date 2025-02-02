#pragma once

#include <arx/unordered_array.hpp>

#include <wv/Types.h>

#include <deque>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <atomic>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

struct JobCounter
{
	std::atomic_int32_t value{ 0 };
};

///////////////////////////////////////////////////////////////////////////////////////

struct Job
{
	typedef void( *JobFunction_t )( const Job*, void* );

	std::string   name      = "";
	JobFunction_t pFunction = nullptr;
	void*         pData     = nullptr;
	JobCounter**  ppCounter = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////

class JobSystem
{
public:

	struct Worker
	{
		std::thread thread;
		std::atomic_bool alive{ true };
	};

///////////////////////////////////////////////////////////////////////////////////////

	JobSystem();

	void initialize( size_t _numWorkers );
	void terminate();

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

	static void _workerThread( wv::JobSystem* _pJobSystem, wv::JobSystem::Worker* _pWorker );

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


	std::vector<Worker*> m_workers;

};

}