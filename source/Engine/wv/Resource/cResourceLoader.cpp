#include "cResourceLoader.h"

#include <wv/Resource/Resource.h>
#include <wv/Memory/FileSystem.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Misc/Time.h>

void threadedLoad( wv::cFileSystem* _pFileSystem, wv::iGraphicsDevice* _pGraphicsDevice, wv::sLoaderInformation* _loaderInfo, wv::sLoadWorker* _worker )
{
	wv::iResource* currentlyLoading = nullptr;

	while( _worker->alive )
	{
		_loaderInfo->loadQueueMutex.lock();
		if ( !_loaderInfo->loadQueue.empty() )
		{
			currentlyLoading = _loaderInfo->loadQueue.front();
			_loaderInfo->loadQueue.pop();
			_worker->state = wv::WV_WORKER_WORKING;
		}
		else
		{
			_worker->state = wv::WV_WORKER_IDLE;
		}
		_loaderInfo->loadQueueMutex.unlock();

		switch ( _worker->state )
		{

		case wv::WV_WORKER_IDLE:
		{
			wv::time::sleepForSeconds( 0.1 );
			break;
		}

		case wv::WV_WORKER_WORKING:
		{
			currentlyLoading->load( _pFileSystem, _pGraphicsDevice );
			_loaderInfo->createQueueMutex.lock();
			
			_loaderInfo->createQueue.push( currentlyLoading );

			currentlyLoading = nullptr;
			_loaderInfo->createQueueMutex.unlock();
		} break;

		}
	}
}

void wv::cResourceLoader::createWorkers( int _count )
{
	for ( int i = 0; i < _count; i++ )
	{
		sLoadWorker* worker = new sLoadWorker();
		worker->thread = std::thread( threadedLoad, m_pFileSystem, m_pGraphicsDevice, &m_info, worker );
		m_workers.push_back( worker );
	}
}

void wv::cResourceLoader::addLoad( iResource* _resource )
{
	if ( _resource->isComplete() )
	{
		wv::Debug::Print( Debug::WV_PRINT_WARN, "Resource %s already loaded\n", _resource->getName().c_str() );
		return;
	}

	m_info.loadQueueMutex.lock();
	m_info.loadQueue.push( _resource );
	m_info.loadQueueMutex.unlock();
}

bool wv::cResourceLoader::isWorking()
{
	m_info.loadQueueMutex.lock();
	m_info.createQueueMutex.lock();
	
	bool loading = !m_info.loadQueue.empty();
	bool creating = !m_info.createQueue.empty();

	bool workingThread = false;
	for ( auto& worker : m_workers )
		workingThread = worker->state == WV_WORKER_WORKING;
	
	m_info.loadQueueMutex.unlock();
	m_info.createQueueMutex.unlock();

	return loading || creating || workingThread;
}
