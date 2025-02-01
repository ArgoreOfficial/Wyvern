#pragma once

#include <deque>
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
	typedef void( *JobFunction_t )( Job*, void* );

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

	[[nodiscard]] Job* createJob( const std::string& _name, Job::JobFunction_t _pFunction, void* _pData = nullptr );
	[[nodiscard]] Job* createJob( Job::JobFunction_t _pFunction, void* _pData = nullptr ) { 
		return createJob( "", _pFunction, _pData );
	}


	void run( Job** _ppJobs, size_t _numJobs = 1, JobCounter** _ppCounter = nullptr );
	void waitForCounter( JobCounter** _ppCounter, int _value );
	void waitForAndFreeCounter( JobCounter** _ppCounter, int _value );

	void freeCounter( JobCounter** _ppCounter );

///////////////////////////////////////////////////////////////////////////////////////

protected:

	static void _workerThread( wv::JobSystem* _pJobSystem, wv::JobSystem::Worker* _pWorker );

	Job* _getNextJob();
	JobCounter* _allocateCounter();

	void _executeJob( Job* _pJob );

	std::mutex m_queueMutex{};
	std::deque<Job*> m_jobQueue{};
	std::vector<Worker*> m_workers;

};

}