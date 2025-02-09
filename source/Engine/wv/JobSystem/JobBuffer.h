#pragma once

#include <stdint.h>
#include <vector>

#include <wv/Types/type_guard.hpp>

namespace wv {

class Job;

class JobBuffer
{
public:
    JobBuffer();

    bool push( Job* _pJob );
    Job* pop();
    Job* steal();

private:
	static const unsigned int g_NUM_JOBS = 4096u * 2;
	
    std::atomic_long m_bottom, m_top;
    std::vector<Job*> m_jobs{};

	std::mutex m_criticalMutex;
};

}
