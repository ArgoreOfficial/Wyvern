#include "world.h"

#include <wv/entity/ecs.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>
#include <wv/rendering/texture.h>

#include <wv/components/camera_component.h>
#include <wv/components/mesh_component.h>
#include <wv/components/orbit_controller_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include <wv/updatable.h>

wv::World::World()
{ 
	m_meshManager     = WV_NEW( MeshManager );
	m_materialManager = WV_NEW( MaterialManager );
	m_textureManager  = WV_NEW( TextureManager );
	m_ecsEngine       = WV_NEW( ECSEngine );

	// register order determines internal ID
	m_ecsEngine->registerComponentType<CameraComponent>();
	m_ecsEngine->registerComponentType<ColliderComponent>();
	m_ecsEngine->registerComponentType<MeshComponent>();
	m_ecsEngine->registerComponentType<OrbitControllerComponent>();
	m_ecsEngine->registerComponentType<RigidBodyComponent>();
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

wv::Entity* wv::World::createEntity( const std::string& _name )
{
	Entity* e = WV_NEW( Entity, _name );
	addEntity( e );
	return e;
}

void wv::World::dispatchUpdateMessage( UpdateMessageType _type )
{
	for ( IUpdatable* updatable : m_updatables )
	{
		switch ( _type )
		{
		case UpdateMessageType_initialize: 
			if( !updatable->m_initalized )
				updatable->onInitialize();
			updatable->m_initalized = true;
			break;

		case UpdateMessageType_shutdown: 
			if ( updatable->m_initalized )
				updatable->onShutdown();
			updatable->m_initalized = false;
			break;

		case UpdateMessageType_preUpdate:  updatable->onPreUpdate();  break;
		case UpdateMessageType_update:     updatable->onUpdate();     break;
		case UpdateMessageType_postUpdate: updatable->onPostUpdate(); break;
		}
	}
}