#include "mesh_component.h"

#include <wv/entity/world.h>
#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>
#include <wv/rendering/texture.h>

#include <wv/debug/log.h>

wv::MeshComponent::~MeshComponent()
{
	
}

void wv::MeshComponent::load( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state != EntityComponentState::UNLOADED );

	m_state = EntityComponentState::LOADING;

	if ( !m_path.empty() )
	{
		m_meshAsset = _ctx.meshManager->get( m_path );

		// TODO: loop through mesh asset textures

		m_material = _ctx.materialManager->get( "Default" );
		m_texture  = _ctx.textureManager->get( "tengil.png" );

		//m_material.setValue( "albedoIndex", m_texture->getImageSlot() );
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

	m_state = EntityComponentState::UNLOADED;
}
