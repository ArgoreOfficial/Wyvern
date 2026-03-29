#include "world.h"

#include <wv/entity/ecs.h>

#include <wv/input/input_system.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>
#include <wv/rendering/texture.h>

#include <wv/rendering/components/mesh_component.h>

wv::World::World()
{ 
	m_meshManager     = WV_NEW( MeshManager );
	m_materialManager = WV_NEW( MaterialManager );
	m_textureManager  = WV_NEW( TextureManager );
	m_ecsEngine       = WV_NEW( ECSEngine );

	m_ecsEngine->registerComponentType<MeshComponent>();
}

wv::World::~World()
{
	for ( Entity* e : m_entities )
		if ( e->archetype )
			m_ecsEngine->removeAllComponents( e );
	
	WV_FREE( m_ecsEngine );

	for ( Entity* e : m_entities )
		WV_FREE( e );
	
	m_textureManager->clearPersistent();
	m_materialManager->clearPersistent();
	m_meshManager->clearPersistent();

	if ( m_viewport )
		WV_FREE( m_viewport );

	// free managers
	WV_FREE( m_textureManager );
	WV_FREE( m_materialManager );
	WV_FREE( m_meshManager );
}
