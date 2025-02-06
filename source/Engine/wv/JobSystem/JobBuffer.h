#pragma once

#include <stdint.h>
#include <vector>

#include <wv/Types/type_guard.hpp>

namespace wv {

class Job;

class JobBuffer
{
public:
    JobBuffer( size_t _size );

    void push( Job* _pJob );
    Job* pop();
    Job* steal();

private:
    uint32_t m_head;
    arx::type_guard<uint32_t> m_tail;
    std::vector<arx::type_guard<Job*>> m_jobs{};
    const size_t m_numJobs{ 0 };
};





}
