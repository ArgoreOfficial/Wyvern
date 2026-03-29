#include "world.h"

#include <wv/input/input_system.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>
#include <wv/rendering/texture.h>

wv::World::World()
{ 
	m_meshManager     = WV_NEW( MeshManager );
	m_materialManager = WV_NEW( MaterialManager );
	m_textureManager  = WV_NEW( TextureManager );
}

wv::World::~World()
{
	m_meshManager->clearPersistent();
	m_materialManager->clearPersistent();
	m_textureManager->clearPersistent();

	if ( m_viewport )
		WV_FREE( m_viewport );

	// free managers
	WV_FREE( m_meshManager );
	WV_FREE( m_materialManager );
	WV_FREE( m_textureManager );
}
