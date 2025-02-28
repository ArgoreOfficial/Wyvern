#pragma once

#include <wv/job/job.h>
#include <wv/job/job_buffer.h>
#include <wv/debug/thread_profiler.h>

#include <thread>
#include <vector>

namespace wv
{

struct JobWorker
{

	std::thread thread{};
	std::atomic_bool isAlive{ true };
	JobBuffer queue{};
	
	bool isHostThread   = false;
	bool isRenderThread = false;
	
#ifndef WV_PACKAGE
	ThreadWorkTrace* workTrace = nullptr;
#endif
};

}