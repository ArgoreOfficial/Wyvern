#include "mesh_component.h"

#include <wv/entity/world.h>
#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>
#include <wv/rendering/texture.h>

#include <wv/editor/mesh_importer_gltf.h>

#include <wv/debug/log.h>

wv::MeshComponent::~MeshComponent()
{
	
}

void wv::MeshComponent::load( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state == EntityComponentState::UNLOADED );

	m_state = EntityComponentState::LOADING;

	if ( !m_path.empty() )
	{
		auto extension = m_path.extension();
		if ( extension == ".gltf" || extension == ".glb" )
		{
			MeshImporterGLTF importer{};
			importer.load( m_path, _ctx.meshManager, _ctx.materialManager, _ctx.textureManager );

			m_meshAsset = importer.getMesh();
			m_materials = importer.getMaterials();
		}
		else
		{
			m_meshAsset = _ctx.meshManager->get( m_path );
			// load materials
		}

		//m_texture  = _ctx.textureManager->get( "tengil.png" );
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
	WV_ASSERT( m_state == EntityComponentState::LOADED );

	m_state = EntityComponentState::UNLOADED;
}
