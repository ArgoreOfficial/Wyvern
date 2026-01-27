#include "mesh_component.h"

#include <wv/entity/world.h>
#include <wv/filesystem/loaders/material_asset_loader.h>
#include <wv/rendering/mesh_manager.h>
#include <wv/debug/log.h>

wv::MeshComponent::~MeshComponent()
{
	
}

void wv::MeshComponent::setMaterial( MaterialType* _material )
{
	if ( m_materialType != nullptr )
		m_materialType->destroyInstance( m_materialInstance );
	
	m_materialType = _material;
	
	if( _material )
		m_materialInstance = _material->createInstance();
}

void wv::MeshComponent::load( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state != EntityComponentState::UNLOADED );

	m_state = EntityComponentState::LOADING;

	if ( !m_path.empty() )
	{
		m_meshAsset     = _ctx.meshAssetLoader->load( m_path );
		m_materialAsset = _ctx.materialAssetLoader->load( "Default" );

		// TODO: loop through mesh asset textures

		if ( MaterialAsset* materialAsset = _ctx.materialAssetLoader->getMaterialAsset( m_materialAsset ) )
		{
			setMaterial( materialAsset->materialTypeDefinition );
		}
	}
	else
	{
		WV_LOG_WARNING( "Mesh Component has no path\n" );
	}

	m_state = EntityComponentState::LOADED;
}

void wv::MeshComponent::unload( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state != EntityComponentState::LOADED );

	if ( m_materialAsset.isValid() )
	{
		setMaterial( nullptr );
		_ctx.materialAssetLoader->unload( m_materialAsset );
	}
	
	m_state = EntityComponentState::UNLOADED;
}
