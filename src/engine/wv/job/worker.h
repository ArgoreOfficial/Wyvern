#pragma once

#include <wv/JobSystem/Job.h>
#include <wv/JobSystem/JobBuffer.h>

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