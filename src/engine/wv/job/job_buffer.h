/**
 * @file JobBuffer.h
 * @author Argore
 * @brief 
 * @version 0.2
 * @date 2025-02-09
 * 
 * @copyright Copyright (c) 2025 Argore
 * 
 */

#pragma once

#include <stdint.h>
#include <vector>
#include <atomic>

#include <wv/types/type_guard.hpp>

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
	
    std::atomic_int32_t m_head, m_tail;
    std::vector<Job*> m_jobs{};

	std::mutex m_criticalMutex;
};

}
