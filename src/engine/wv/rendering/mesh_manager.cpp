#include "mesh_manager.h"

#include <wv/filesystem/asset_manager.h>

wv::ResourceID wv::MeshManager::load( const std::filesystem::path& _path )
{
	ResourceID res{};

	if ( m_assetManager->containsAsset( _path ) )
	{
		res = m_assetManager->getAsset( _path );
	}
	else
	{
		res = m_meshAssets.emplace( _path );
	}

	if( res.isValid() )
		m_assetManager->notifyAssetLoad( res, _path );

	return res;
}

void wv::MeshManager::unload( ResourceID _resource )
{
	if ( !m_meshAssets.contains( _resource ) )
	{
		return;
	}

	std::filesystem::path path = m_meshAssets.at( _resource ).m_path;
	int refcount = m_assetManager->notifyAssetUnload( path );

	if ( refcount <= 0 )
	{
		MeshAsset& asset = m_meshAssets.at( _resource );
		asset = {};
		m_meshAssets.erase( _resource );
	}
}

wv::MeshAsset* wv::MeshManager::getMeshAsset( ResourceID _assetID ) const
{
	if ( !m_meshAssets.contains( _assetID ) )
		return nullptr;
	return const_cast<MeshAsset*>( &m_meshAssets.at( _assetID ) );
}

wv::ResourceID wv::MeshManager::getGPUAllocation( ResourceID _assetID ) const
{
	if ( !m_meshAssets.contains( _assetID ) )
		return {};
	return m_meshAssets.at( _assetID ).getGPUAllocation();
}
