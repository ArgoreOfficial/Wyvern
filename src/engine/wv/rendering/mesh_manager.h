#pragma once

#include <wv/filesystem/asset_loader.h>

#include <wv/rendering/mesh.h>
#include <wv/types.h>

#include <unordered_map>

namespace wv {

// generic read through cache?

class MeshManager
{
public:
	MeshManager( IFileSystem* _filesystem, AssetManager* _assetManager ) :
		m_filesystem{ _filesystem }
	{ }

	Ref<MeshAsset> load( const std::filesystem::path& _path ) {
		auto it = m_managed.find( _path );
		if ( it != m_managed.end() )
			return it->second.lock();

		Ref<MeshAsset> ref = std::make_shared<MeshAsset>( _path );
		m_managed.emplace( _path, ref );
		return ref;
	}
	
private:
	IFileSystem* m_filesystem = nullptr;

	std::unordered_map<std::filesystem::path, WeakRef<MeshAsset>> m_managed;

};

}