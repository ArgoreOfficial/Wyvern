#pragma once

#include <vector>
#include <thread>
#include <mutex>

namespace wv
{
	class iResource;
	class cFileSystem;

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

		cResourceLoader( cFileSystem* _pFileSystem ) : 
			m_pFileSystem{ _pFileSystem }
		{ }

		void addLoad( iResource* _resource );
		void dispatchLoad();

		void update();
		bool isLoading();

	private:
		cFileSystem* m_pFileSystem;

		std::vector<iResource*> m_loadQueue;
		std::vector<sLoadWorker*> m_workers;
	};
}