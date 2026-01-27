#pragma once

#include <wv/filesystem/asset_loader.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

#include <wv/helpers/unordered_array.hpp>
#include <wv/rendering/mesh.h>

#include <vector>

namespace wv {

class MeshAsset;

// generic read through cache?

class MeshManager
{
public:
	MeshManager( IFileSystem* _filesystem, AssetManager* _assetManager ) :
		m_filesystem{ _filesystem },
		m_assetManager{ _assetManager }
	{ }

	ResourceID add( const MeshAsset& _meshAsset ) {
		return m_meshAssets.emplace( _meshAsset );
	}

	ResourceID load  ( const std::filesystem::path& _path );
	void       unload( ResourceID _resource );

	MeshAsset* getMeshAsset( ResourceID _assetID ) const;
	ResourceID getGPUAllocation( ResourceID _assetID ) const;

private:

	AssetManager* m_assetManager = nullptr;
	IFileSystem* m_filesystem = nullptr;

	unordered_array<ResourceID, MeshAsset> m_meshAssets;

};

}