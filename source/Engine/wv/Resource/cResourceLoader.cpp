#include "cResourceLoader.h"

#include <wv/Resource/Resource.h>
#include <wv/Memory/MemoryDevice.h>

void threadedLoad( wv::cFileSystem* _pFileSystem, std::vector<wv::iResource*> _queue, wv::sLoadWorker* _worker )
{
	for ( size_t i = 0; i < _queue.size(); i++ )
	{
		_queue[ i ]->load( _pFileSystem );

		_worker->mutex.lock();
		_worker->loadedResources.push_back( _queue[ i ] );
		_worker->mutex.unlock();
	}

	_worker->mutex.lock();
	_worker->done = true;
	_worker->mutex.unlock();
}

void wv::cResourceLoader::addLoad( iResource* _resource )
{
	m_loadQueue.push_back( _resource );
}

void wv::cResourceLoader::dispatchLoad()
{
	sLoadWorker* worker = new sLoadWorker();
	worker->thread = std::thread( threadedLoad, m_pFileSystem, m_loadQueue, worker );

	m_workers.push_back( worker );

	m_loadQueue.clear();
}

void wv::cResourceLoader::update()
{
	for ( size_t i = 0; i < m_workers.size(); i++ )
	{
		sLoadWorker& worker = *m_workers[ i ];

		if ( !worker.mutex.try_lock() )
			continue;
		
		for ( size_t i = 0; i < worker.loadedResources.size(); i++ )
			worker.loadedResources[ i ]->create();
	
		worker.loadedResources.clear();

		if ( worker.done )
		{
			worker.mutex.unlock();

			worker.thread.join();

			delete &worker;
			m_workers.erase( m_workers.begin() + i );
			i--;
		}

		worker.mutex.unlock();
	}

}

bool wv::cResourceLoader::isLoading()
{

	return !m_workers.empty();
}
