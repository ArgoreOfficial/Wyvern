#pragma once

#include <vector>
#include <thread>
#include <mutex>

namespace wv
{
	class iResource;
	class cFileSystem;
	class iGraphicsDevice;

	struct sLoadWorker
	{
		std::thread thread;
		std::mutex mutex;
		std::vector<iResource*> loadedResources;
		bool done = false;
	};

	class cResourceLoader
	{
	public:

		cResourceLoader( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) : 
			m_pFileSystem{ _pFileSystem },
			m_pGraphicsDevice{ _pGraphicsDevice }
		{ }

		void addLoad( iResource* _resource );
		void dispatchLoad();

		void update();
		bool isLoading();

	private:
		cFileSystem* m_pFileSystem;
		iGraphicsDevice* m_pGraphicsDevice;

		std::vector<iResource*> m_loadQueue;
		std::vector<sLoadWorker*> m_workers;
	};
}