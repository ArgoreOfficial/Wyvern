#pragma once

#include <wv/types.h>

#include <filesystem>

namespace wv {

class AssetManager;
class IFileSystem;

class IAssetLoader
{
public:

	IAssetLoader( IFileSystem* _filesystem, AssetManager* _assetManager ) :
		m_filesystem{ _filesystem },
		m_assetManager{ _assetManager } 
	{ }

	virtual ~IAssetLoader() { 
		m_assetManager = nullptr;
	}

	virtual ResourceID load( const std::filesystem::path& _path ) = 0;
	virtual void unload( ResourceID _resource ) = 0;

protected:

	AssetManager* m_assetManager = nullptr;
	IFileSystem*  m_filesystem   = nullptr;

};

}