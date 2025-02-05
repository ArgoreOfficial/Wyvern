#pragma once

#include "Job.h"

namespace wv
{

class JobWorker
{
public:

	void create();
	void destroy();

	Job* stealJob();

private:

	Job* _getJob();
	Job* _pushJob();

	std::queue<Job*> m_jobQueue;
};

}