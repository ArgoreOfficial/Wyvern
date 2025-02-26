#pragma once

#include <wv/job/job.h>
#include <wv/job/job_buffer.h>

#include <thread>

namespace wv
{

struct JobWorker
{
	std::thread thread{};
	std::atomic_bool isAlive{ true };
	JobBuffer queue{};
	
	bool isHostThread   = false;
	bool isRenderThread = false;
};

}