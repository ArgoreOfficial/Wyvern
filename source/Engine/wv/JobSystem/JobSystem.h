#pragma once

#include <arx/unordered_array.hpp>

#include <wv/Types.h>

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
	typedef void( *JobFunction_t )( const Job, void* );

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

	typedef arx::strong_type<uint16_t, struct JobID_t> JobID;

///////////////////////////////////////////////////////////////////////////////////////

	JobSystem();

	void initialize( size_t _numWorkers );
	void terminate();

	[[nodiscard]] JobID createJob( const std::string& _name, Job::JobFunction_t _pFunction, JobCounter** _ppCounter = nullptr, void* _pData = nullptr );
	[[nodiscard]] JobID createJob( Job::JobFunction_t _pFunction, JobCounter** _ppCounter = nullptr, void* _pData = nullptr )
	{
		return createJob( "", _pFunction, _ppCounter, _pData );
	}


	void run( JobID* _pJobs, size_t _numJobs = 1 );
	void waitForCounter( JobCounter** _ppCounter, int _value );
	void waitForAndFreeCounter( JobCounter** _ppCounter, int _value );

	void freeCounter( JobCounter** _ppCounter );

///////////////////////////////////////////////////////////////////////////////////////

protected:

	static void _workerThread( wv::JobSystem* _pJobSystem, wv::JobSystem::Worker* _pWorker );

	JobID _getNextJob();
	JobCounter* _allocateCounter();

	void _executeJob( JobID _job );

	std::mutex m_queueMutex{};

	arx::unordered_array<JobID, Job> m_jobPool{};
	std::deque<JobID> m_jobQueue{};


	std::vector<Worker*> m_workers;

};

}