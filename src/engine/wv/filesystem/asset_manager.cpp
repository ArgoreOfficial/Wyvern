#include "asset_manager.h"

#include <wv/debug/log.h>

int wv::AssetManager::notifyAssetLoad( ResourceID _resourceID, const std::filesystem::path& _path )
{
	AssetRef& asset = m_managedAssets[ _path ];
	if ( asset.refs == 0 ) // new asset
	{
		Debug::Print( "Loaded asset '%s'\n", _path.string().c_str() );
		asset.asset = _resourceID;
	}

	asset.refs++;

	return asset.refs;
}

int wv::AssetManager::notifyAssetUnload( const std::filesystem::path& _path )
{
	if ( !m_managedAssets.contains( _path ) )
	{
		// error
		return 0;
	}

	AssetRef& asset = m_managedAssets.at( _path );
	asset.refs--;
	
	int refcount = asset.refs;

	if ( refcount <= 0 ) // erase asset
	{
		Debug::Print( "Unloaded asset '%s'\n", _path.string().c_str() );
		m_managedAssets.erase( _path );
	}
	
	return refcount;
}
