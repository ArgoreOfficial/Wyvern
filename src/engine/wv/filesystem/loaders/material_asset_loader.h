#pragma once

#include <wv/filesystem/asset_loader.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

#include <wv/helpers/unordered_array.hpp>
#include <wv/resource_id.h>

#include <vector>

namespace wv {

class Renderer;
class MaterialType;

struct MaterialAsset
{
	std::filesystem::path path;
	
	std::filesystem::path vertShaderPath;
	std::filesystem::path fragShaderPath;

	std::vector<uint8_t> vertCode;
	std::vector<uint8_t> fragCode;
	MaterialType* materialTypeDefinition = nullptr;

	bool isLoaded = false;
};

class MaterialAssetLoader : public IAssetLoader
{
public:
	MaterialAssetLoader( IFileSystem* _filesystem, AssetManager* _assetManager, Renderer* _renderer ) :
		IAssetLoader( _filesystem, _assetManager ),
		m_renderer{ _renderer }
	{ }

	~MaterialAssetLoader();

	ResourceID add( const std::filesystem::path& _vpath, MaterialAsset& _asset );

	virtual ResourceID load  ( const std::filesystem::path& _path ) override;
	virtual void       unload( ResourceID _resource )               override;

	MaterialAsset* getMaterialAsset( ResourceID _resource ) const {
		if ( !m_materialAssets.contains( _resource ) )
			return nullptr;
		return const_cast<MaterialAsset*>( &m_materialAssets.at(_resource) );
	}

private:

	void loadMaterial( MaterialAsset& _asset );

	Renderer* m_renderer = nullptr;

	unordered_array<ResourceID, MaterialAsset> m_materialAssets;
};

}