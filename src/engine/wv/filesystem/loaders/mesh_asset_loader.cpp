#include "mesh_asset_loader.h"

#include <wv/filesystem/asset_manager.h>

wv::ResourceID wv::MeshAssetLoader::load( const std::filesystem::path& _path )
{
	// TODO: actual mesh loading lmfao
	ResourceID res{};

	if ( !m_assetManager->containsAsset( _path ) )
	{
		res = m_assetManager->getAsset( _path );
	}
	else
	{
		MeshAsset asset{};

		asset.vertexPositions = {
			{ -1, -1, -1 },
			{  1, -1, -1 },
			{  1,  1, -1 },
			{ -1,  1, -1 },
			{ -1, -1,  1 },
			{  1, -1,  1 },
			{  1,  1,  1 },
			{ -1,  1,  1 }
		};

		asset.indices = {
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		asset.vertexColours = {
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f },
		};

		res = m_meshAssets.emplace( asset );
	}

	m_assetManager->notifyAssetLoad( res, _path );
	return res;
}

void wv::MeshAssetLoader::unload( ResourceID _resource )
{
	if ( !m_meshAssets.contains( _resource ) )
	{
		return;
	}

	std::filesystem::path path = m_meshAssets.at( _resource ).path;
	int refcount = m_assetManager->notifyAssetUnload( path );

	if ( refcount <= 0 )
	{
		// TODO: actual mesh unloading
		m_meshAssets.erase( _resource );
	}
}
