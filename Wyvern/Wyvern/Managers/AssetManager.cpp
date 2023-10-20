#include "AssetManager.h"
#include <thread>

using namespace WV;

void cAssetManager::internalUnloadAll()
{
	WVDEBUG( "Destroying Assets:" );
	
	for ( auto itr = m_loadedAssets.begin(); itr != m_loadedAssets.end(); itr++ )
	{
		WVDEBUG( "Destroying %s", Asset::assetTypeToString( itr->second->assetType ).c_str() );
		delete itr->second;
	}
	m_loadedAssets.clear();

	for ( auto itr = m_loadedMeshes.begin(); itr != m_loadedMeshes.end(); itr++ )
	{
		WVDEBUG( "Destroying mesh" );
		delete itr->second;
	}
	m_loadedAssets.clear();
}

void WV::cAssetManager::load( std::string _path, eAssetType _type )
{
	if ( _type == eAssetType::AUTO )
		_type = Asset::getAssetTypeFromFile( _path );
	
	sAsset queuedAsset{_path, _type, nullptr, 0};
	
	getInstance().m_loadQueue.push_back( queuedAsset );
}

const sAsset* WV::cAssetManager::getAsset( std::string _name )
{
	cAssetManager& instance = getInstance();

	if ( !instance.m_hasLoaded )
	{
		WVFATAL( "Attempted to get asset before loading has finished!" );
		return nullptr;
	}
	
	if ( !instance.m_loadedAssets.count( _name ) )
	{
		WVERROR( "Asset not found: %s", _name.c_str() );
		return nullptr;
	}

	return instance.m_loadedAssets[ _name ];
}

// ------------------------- TEMPORARY ------------------------- //

Mesh* WV::cAssetManager::getMesh( std::string _name )
{
	cAssetManager& instance = getInstance();

	if ( !instance.m_hasLoaded )
	{
		WVFATAL( "Attempted to get asset before loading has finished!" );
		return nullptr;
	}

	if ( !instance.m_loadedMeshes.count( _name ) )
	{
		WVERROR( "Mesh not found: %s", _name.c_str() );
		return nullptr;
	}

	return instance.m_loadedMeshes[ _name ];
}

// ------------------------- TEMPORARY ------------------------- //

void WV::cAssetManager::loadQueuedAssetThread( cAssetManager* _instance )
{
	cAssetManager& instance = *_instance;

	while ( instance.m_loadQueue.size() > 0 )
	{
		sAsset current = instance.m_loadQueue.front();
		std::string filename = Filesystem::getFilenameFromPath( current.path );

		if ( current.assetType == eAssetType::MESH )
		{
			Mesh* mesh = new Mesh();
			mesh->loadOBJ( current.path );
			instance.m_loadedMeshes[ filename ] = mesh;
		}
		else
		{
			sAsset* loadedAsset = new sAsset();
			std::vector<char>* data = Filesystem::loadByteArrayFromPath( current.path );
		
			loadedAsset->path = current.path;
			loadedAsset->assetType = current.assetType;
			loadedAsset->size = data->size();

			uint32_t size = data->size();
			loadedAsset->data = (unsigned char*)malloc( size + 1 );
			memcpy( loadedAsset->data, data->data(), size );
			instance.m_loadedAssets[ filename ] = loadedAsset;
		}

		instance.m_loadQueue.erase( instance.m_loadQueue.begin() );

		std::string assetype = Asset::assetTypeToString( current.assetType );
		WVDEBUG( "Loaded %s ( %s )", filename.c_str(), assetype.c_str());
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

