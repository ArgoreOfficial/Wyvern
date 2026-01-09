#pragma once

#include <wv/types.h>

#include <filesystem>

namespace wv {

class AssetManager;

class IAssetLoader
{
public:

	IAssetLoader( AssetManager* _assetManager ) : 
		m_assetManager{ _assetManager } 
	{ }

	virtual ~IAssetLoader() { 
		m_assetManager = nullptr;
	}

	virtual ResourceID load( const std::filesystem::path& _path ) = 0;
	virtual void unload( ResourceID _resource ) = 0;

protected:

	AssetManager* m_assetManager = nullptr;

};

}