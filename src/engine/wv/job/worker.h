#pragma once

#include <wv/job/job.h>
#include <wv/job/job_buffer.h>

#include <thread>

namespace wv
{

struct JobWorker
{
	std::thread mThread{};
	std::atomic_bool mIsAlive{ true };
	JobBuffer mQueue{};
	size_t victimIndex = 0;
};

}