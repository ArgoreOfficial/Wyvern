#pragma once

#include <wv/filesystem/asset_loader.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

#include <wv/helpers/unordered_array.hpp>

#include <vector>

namespace wv {

struct GeometrySurface
{
	uint32_t startIndex;
	uint32_t indexCount;
	uint32_t vertexCount;
};

struct MeshAsset
{
	std::filesystem::path path;
	std::vector<GeometrySurface> surfaces;
	
	std::vector<uint16_t> indices;

	std::vector<Vector3f> vertexPositions;
	std::vector<Vector3f> vertexNormals;
	std::vector<Vector2f> vertexUVs;
	std::vector<Vector3f> vertexColours;

};

class MeshAssetLoader : public IAssetLoader
{
public:
	MeshAssetLoader( IFileSystem* _filesystem, AssetManager* _assetManager ) : IAssetLoader( _filesystem, _assetManager ) { }

	ResourceID add( const MeshAsset& _meshAsset ) {
		return m_meshAssets.emplace( _meshAsset );
	}

	virtual ResourceID load  ( const std::filesystem::path& _path ) override;
	virtual void       unload( ResourceID _resource )               override;

	MeshAsset* getMeshAsset( ResourceID _resource ) const {
		if ( !m_meshAssets.contains( _resource ) )
			return nullptr;
		return const_cast<MeshAsset*>( &m_meshAssets.at(_resource) ); 
	}

private:

	unordered_array<ResourceID, MeshAsset> m_meshAssets;

};

}