#pragma once

#include <wv/filesystem/asset_loader.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

#include <wv/helpers/unordered_array.hpp>

#include <vector>

namespace wv {

struct MeshAsset
{
	std::filesystem::path path;

	std::vector<uint16_t> indices;

	std::vector<Vector3f> vertexPositions;
	std::vector<Vector3f> vertexNormals;
	std::vector<Vector2f> vertexUVs;
	std::vector<Vector3f> vertexColours;
};

class MeshAssetLoader : public IAssetLoader
{
public:
	MeshAssetLoader( AssetManager* _assetManager ) : IAssetLoader( _assetManager ) { }

	ResourceID add( const MeshAsset& _meshAsset ) {
		return m_meshAssets.emplace( _meshAsset );
	}

	virtual ResourceID load  ( const std::filesystem::path& _path ) override;
	virtual void       unload( ResourceID _resource )               override;

	MeshAsset* getMeshAsset( ResourceID _resource ) const {
		if ( !m_meshAssets.contains( _resource ) )
			return nullptr;
		return &m_meshAssets.at( _resource );
	}

private:

	unordered_array<ResourceID, MeshAsset> m_meshAssets;

};

}