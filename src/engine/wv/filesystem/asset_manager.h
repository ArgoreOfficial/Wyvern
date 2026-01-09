#pragma once

#include <wv/types.h>

#include <filesystem>
#include <unordered_map>

namespace wv {

class AssetManager
{
public:
	struct AssetRef
	{
		ResourceID asset;
		int refs = 0;
	};

	AssetManager() = default;

	bool       containsAsset( const std::filesystem::path& _path ) const { return m_managedAssets.contains( _path ); }
	ResourceID getAsset     ( const std::filesystem::path& _path ) const { return m_managedAssets.at( _path ).asset; }

	/**
	 * @brief Notify manager of asset load
	 * @param _resourceID The resource ID
	 * @param _path Path to the resource
	 * @return The number of references to asset
	 */
	int notifyAssetLoad( ResourceID _resourceID, const std::filesystem::path& _path );

	/**
	 * @brief Notify manager of asset unload
	 * @param _path Path to the resource
	 * @return The number of references to asset
	 */
	int notifyAssetUnload( const std::filesystem::path& _path );

private:

	std::unordered_map<std::filesystem::path, AssetRef> m_managedAssets;
};

}