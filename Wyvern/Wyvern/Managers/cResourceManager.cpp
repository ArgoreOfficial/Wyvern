#include "cResourceManager.h"

#include <thread>

using namespace wv;

void wv::cResourceManager::loadQueuedAssetThread( cResourceManager* _instance )
{

	cResourceManager& instance = *_instance;

	for ( size_t i = 0; i < instance.m_resourceQueue.size(); i++ )
	{
		cResourceHandle handle = instance.m_resourceQueue[ i ];
		unsigned int uuid = handle.getUUID();
		std::string path = handle.getPath();

		instance.m_resources[ uuid ]->load( path );

	}

	WV_DEBUG( "Loading complete. Killing thread..." );
	
	instance.m_assetManagerMutex.lock();
	
	instance.m_hasLoaded = true;
	instance.m_isLoading = false;
	
	instance.m_assetManagerMutex.unlock();

}

void wv::cResourceManager::pushResource( cResourceHandle& _handle, iResource* _resource )
{

	unsigned int uuid = _handle.getUUID();
	m_resources[ uuid ] = _resource;
	m_resourceQueue.push_back( _handle );

}

std::thread* cResourceManager::loadResources()
{

	m_isLoading = true;

	WV_DEBUG( "Loading assets. Creating thread..." );

	m_assetLoadThread = new std::thread( cResourceManager::loadQueuedAssetThread, this );
	return m_assetLoadThread;

}

void wv::cResourceManager::createResources( void )
{

	for ( size_t i = 0; i < m_resourceQueue.size(); i++ )
	{
		cResourceHandle handle = m_resourceQueue[ i ];
		m_resources[ handle.getUUID() ]->create();
	}

}

void cResourceManager::unloadAllResources()
{
	
	WV_DEBUG( "Unloaded assets" );

}

tModelHandle wv::cResourceManager::loadModel( std::string _path )
{

	tModelHandle handle{ _path };
	cModel* resource = new cModel();
	
	m_models[ handle.getUUID() ] = resource;
	pushResource( handle, resource );

	return handle;

}

tTextureHandle wv::cResourceManager::loadTexture( std::string _path )
{

	tTextureHandle handle{ _path };
	cTexture2D* resource = new cTexture2D();

	m_textures[ handle.getUUID() ] = resource;
	pushResource( handle, resource );

	return handle;

}

bool cResourceManager::isLoading()
{

	bool out = false;
	m_assetManagerMutex.lock();
	out = m_isLoading;
	m_assetManagerMutex.unlock();

	return out;

}

unsigned int wv::cResourceManager::getNewUUID( void )
{
	m_lastUUID++;
	return m_lastUUID;
}

cModel* wv::cResourceManager::getModel( tModelHandle _handle )
{

	unsigned long long uuid = _handle.getUUID();
	
	if ( m_models.count( uuid ) )
		return m_models[ uuid ];

	return nullptr;

}

cTexture2D* wv::cResourceManager::getTexture( tTextureHandle _handle )
{
	
	unsigned long long uuid = _handle.getUUID();
	
	if ( m_textures.count( uuid ) )
		return m_textures[ uuid ];

	return nullptr;

}

