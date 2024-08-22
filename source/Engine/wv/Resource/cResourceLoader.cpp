#include "cResourceLoader.h"

#include <wv/Resource/Resource.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Device/GraphicsDevice.h>

#include <chrono>

void threadedLoad( wv::cFileSystem* _pFileSystem, wv::sLoaderInformation* _loaderInfo, wv::sLoadWorker* _worker )
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
			
			/// TODO: wv::time
		#ifdef WV_PLATFORM_WINDOWS
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) ); break;
		#elif WV_PLATFORM_PSVITA
		#endif
		
		case wv::WV_WORKER_WORKING:
		{
			currentlyLoading->load( _pFileSystem );
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
		worker->thread = std::thread( threadedLoad, m_pFileSystem, &m_info, worker );
		m_workers.push_back( worker );
	}
}

void wv::cResourceLoader::addLoad( iResource* _resource )
{
	if ( _resource->isLoaded() )
	{
		wv::Debug::Print( Debug::WV_PRINT_WARN, "Resource %s already loaded\n", _resource->getName().c_str() );
		return;
	}

	m_info.loadQueueMutex.lock();
	m_info.loadQueue.push( _resource );
	m_info.loadQueueMutex.unlock();
}

void wv::cResourceLoader::update()
{
	m_info.createQueueMutex.lock();

	while ( !m_info.createQueue.empty() )
	{
		iResource* resource = m_info.createQueue.front();

		if ( !resource->isCreated() )
			resource->create( m_pGraphicsDevice );
		else
			wv::Debug::Print( Debug::WV_PRINT_WARN, "Resource %s already created\n", resource->getName().c_str() );
		
		m_info.createQueue.pop();
	}
	m_info.createQueueMutex.unlock();
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
