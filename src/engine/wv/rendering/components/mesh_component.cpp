#include "mesh_component.h"

#include <wv/entity/world.h>
#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>

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
		m_meshAsset     = _ctx.meshManager->get( m_path );
		m_materialAsset = _ctx.materialManager->get( "Default" );

		// TODO: loop through mesh asset textures

		if ( m_materialAsset )
			setMaterial( m_materialAsset->materialTypeDefinition );
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

	setMaterial( nullptr );
	
	m_state = EntityComponentState::UNLOADED;
}
