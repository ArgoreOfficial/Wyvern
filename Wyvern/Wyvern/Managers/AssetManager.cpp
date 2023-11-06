#include "AssetManager.h"
#include <thread>

using namespace WV;

void cAssetManager::internalUnloadAll()
{
	WVDEBUG( "Unloaded assets" );
}

void WV::cAssetManager::addAssetToLoadQueue( IAsset* _asset )
{
	if ( !_asset )
		return;

	getInstance().m_loadQueue.push_back( _asset );
}

void WV::cAssetManager::loadQueuedAssetThread( cAssetManager* _instance )
{
	cAssetManager& instance = *_instance;

	while ( instance.m_loadQueue.size() > 0 )
	{
		IAsset* current = instance.m_loadQueue.front();
		current->load();
		instance.m_loadQueue.erase( instance.m_loadQueue.begin() );
	}

	instance.m_assetManagerMutex.lock();
	WVDEBUG( "Loading complete. Killing thread..." );
	instance.m_hasLoaded = true;
	instance.m_isLoading = false;
	instance.m_assetManagerMutex.unlock();
}

std::thread* cAssetManager::internalLoadQueued()
{
	m_isLoading = true;

	WVDEBUG( "Loading assets. Creating thread..." );

	cAssetManager& instance = getInstance();
	instance.m_assetLoadThread = new std::thread( cAssetManager::loadQueuedAssetThread, &instance );
	return m_assetLoadThread;
}

bool cAssetManager::internalIsLoading()
{
	bool out = true;
	m_assetManagerMutex.lock();
	out = m_isLoading;
	m_assetManagerMutex.unlock();

	return out;
}

