#include "material_asset_loader.h"

#include <wv/debug/log.h>
#include <wv/filesystem/asset_manager.h>
#include <wv/filesystem/file_system.h>
#include <wv/rendering/material.h>

#include <wv/application.h>
#include <wv/rendering/renderer.h>

#include <wv/memory/memory.h>

wv::MaterialAssetLoader::~MaterialAssetLoader()
{
	//// clear out all remaining assets

	//std::set<ResourceID> keys;

	//do
	//{
	//	keys = m_materialAssets.keys();
	//	for ( auto k : keys )
	//		unload( k );
	//} while ( !keys.empty() );

}

wv::ResourceID wv::MaterialAssetLoader::add( const std::filesystem::path& _vpath, MaterialAsset& _asset )
{
	if ( m_assetManager->containsAsset( _vpath ) )
	{
		WV_LOG_WARNING( "Material with vpath '%s' already exists\n", _vpath.string().c_str() );
		return m_assetManager->getAsset( _vpath );
	}

	_asset.path = _vpath;

	if ( !_asset.isLoaded )
		loadMaterial( _asset );
	
	ResourceID res = m_materialAssets.emplace( _asset );
	m_assetManager->notifyAssetLoad( res, _vpath, false );
	
	return res;
}

wv::ResourceID wv::MaterialAssetLoader::load( const std::filesystem::path& _path )
{
	ResourceID res{};
	
	if ( m_assetManager->containsAsset( _path ) )
	{
		res = m_assetManager->getAsset( _path );
	}
	else
	{
		WV_LOG_WARNING( __FUNCTION__ " not implemented" );
		// loadMaterial( res );
	}

	if ( res.isValid() )
		m_assetManager->notifyAssetLoad( res, _path );

	return res;
}

void wv::MaterialAssetLoader::unload( ResourceID _resource )
{ 
	if ( !m_materialAssets.contains( _resource ) )
	{
		return;
	}

	std::filesystem::path path = m_materialAssets.at( _resource ).path;
	int refcount = m_assetManager->notifyAssetUnload( path );

	if ( refcount <= 0 )
	{
		MaterialAsset& asset = m_materialAssets.at( _resource );
		
		if ( asset.materialTypeDefinition )
		{
			m_renderer->destroyPipeline( asset.materialTypeDefinition->getPipeline() );

			WV_FREE( asset.materialTypeDefinition );
		}

		m_materialAssets.erase( _resource );
	}
}

void wv::MaterialAssetLoader::loadMaterial( MaterialAsset& _asset )
{ 
	_asset.fragCode = m_filesystem->loadEntireFile( _asset.fragShaderPath );
	_asset.vertCode = m_filesystem->loadEntireFile( _asset.vertShaderPath );

	ResourceID pipeline = m_renderer->createPipeline( 
		(uint32_t*)_asset.vertCode.data(), _asset.vertCode.size(),
		(uint32_t*)_asset.fragCode.data(), _asset.fragCode.size()
	);

#ifndef WV_DEBUG
	_asset.fragCode.clear();
	_asset.vertCode.clear();
#endif

	_asset.materialTypeDefinition = WV_NEW( MaterialType, pipeline );
	_asset.materialTypeDefinition->addSpan( "albedoIndex", UNIFORM_TYPE_TEXTURE );
	
	_asset.isLoaded = true;
}
