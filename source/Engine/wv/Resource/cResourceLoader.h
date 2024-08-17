#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>

namespace wv
{
	class iResource;
	class cFileSystem;
	class iGraphicsDevice;

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
		std::queue<iResource*> createQueue;
		std::mutex loadQueueMutex;
		std::mutex createQueueMutex;
	};

	class cResourceLoader
	{
	public:

		cResourceLoader( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) : 
			m_pFileSystem{ _pFileSystem },
			m_pGraphicsDevice{ _pGraphicsDevice }
		{ 
			createWorkers( 5 );
		}

		void createWorkers( int _count );

		void addLoad( iResource* _resource );
		
		void update();
		bool isWorking();

	private:
		cFileSystem* m_pFileSystem;
		iGraphicsDevice* m_pGraphicsDevice;

		sLoaderInformation m_info;
		std::vector<sLoadWorker*> m_workers;
	};
}