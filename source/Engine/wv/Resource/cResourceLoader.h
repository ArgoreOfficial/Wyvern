#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>

namespace wv
{
	class iResource;
	class cFileSystem;
	class iLowLevelGraphics;

	enum eLoadWorkerState
	{
		WV_WORKER_IDLE,
		WV_WORKER_WORKING
	};

	struct sLoadWorker
	{
		std::thread thread;
		eLoadWorkerState state = WV_WORKER_IDLE;
		bool alive = true;
	};

	struct sLoaderInformation
	{
		std::queue<iResource*> loadQueue;
		std::mutex loadQueueMutex;
	};

	class cResourceLoader
	{
	public:

		cResourceLoader( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ) : 
			m_pFileSystem{ _pFileSystem },
			m_pLowLevelGraphics{ _pLowLevelGraphics }
		{ 
		#ifndef WV_PLATFORM_PSVITA
			createWorkers( 10 );
		#else
			createWorkers( 1 );
		#endif
		}

		void createWorkers( int _count );

		void addLoad( iResource* _resource );
		
		bool isWorking();

	private:
		cFileSystem* m_pFileSystem;
		iLowLevelGraphics* m_pLowLevelGraphics;

		sLoaderInformation m_info;
		std::vector<sLoadWorker*> m_workers;
	};
}