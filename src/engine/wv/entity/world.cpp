#include "world.h"

#include <wv/entity/ecs.h>

#include <wv/rendering/mesh.h>
#include <wv/rendering/texture.h>

#include <wv/components/camera_component.h>
#include <wv/components/mesh_component.h>
#include <wv/components/orbit_controller_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include <wv/updatable.h>
#include <wv/application.h>

wv::World::World()
{ 
	m_textureManager  = WV_NEW( TextureManager );
	m_ecsEngine       = WV_NEW( ECSEngine );

	// register order determines internal ID
	registerComponentType<CameraComponent>();
	registerComponentType<ColliderComponent>();
	registerComponentType<MeshComponent>();
	registerComponentType<OrbitControllerComponent>();
	registerComponentType<RigidBodyComponent>();
}

wv::World::~World()
{
	destroyAllEntities();

	WV_FREE( m_ecsEngine );

	m_textureManager->clearPersistent();
	
	if ( m_viewport )
		WV_FREE( m_viewport );

	// free managers
	WV_FREE( m_textureManager );
	
}

void wv::World::destroyAllEntities()
{
	if ( m_entities.empty() )
		return;

	for ( Entity* e : m_entities )
		if ( e->archetype )
			removeAllComponents( e );

	updateComponentChanges();

	for ( Entity* e : m_entities )
		WV_FREE( e );

	m_entities.clear();
}

wv::Entity* wv::World::createEntity( const std::string& _name )
{
	Entity* e = WV_NEW( Entity, _name );
	addEntity( e );
	return e;
}

void wv::World::updateFrameData( double _deltaTime, double _physicsDeltaTime )
{
	m_updateContext = {};
	m_updateContext.inputSystem = getApp()->getInputSystem();
	m_updateContext.actionEventQueue = getApp()->getInputSystem()->getActionEventQueue();

	for ( auto& [i, sys] : m_ecsEngine->m_systemIndexMap )
	{
		sys->deltaTime = _deltaTime;
		sys->physicsDeltaTime = _physicsDeltaTime;
		sys->updateContext = &m_updateContext;
	}
}

void wv::World::dispatchUpdateMessage( UpdateEventType _type )
{
	for ( IUpdatable* updatable : m_updatables )
	{
		// init, shutdown, and debug rendering will always run
		switch ( _type )
		{
		case UpdateEvent_initialize:
			if ( !updatable->m_initialized )
				updatable->onInitialize();
			updatable->m_initialized = true;
			break;

		case UpdateEvent_shutdown:
			if ( updatable->m_initialized )
				updatable->onShutdown();
			updatable->m_initialized = false;
			break;

		case UpdateEvent_debugRender:
			if ( updatable->getDebugRenderEnabled() )
				updatable->onDebugRender();
			break;

		case UpdateEvent_editorRender:
			if ( updatable->getEditorRenderEnabled() )
				updatable->onEditorRender();
			break;
		}

		bool runInRuntime = updatable->getUpdateMode() & UpdateMode_runtime;
		bool runInEditor  = updatable->getUpdateMode() & UpdateMode_editor;

		if ( m_isInEditorState && !runInEditor )
			continue;
		if ( !m_isInEditorState && !runInRuntime )
			continue;

		switch ( _type )
		{
		case UpdateEvent_preUpdate:  updatable->onPreUpdate();  break;
		case UpdateEvent_update:     updatable->onUpdate();     break;
		case UpdateEvent_postUpdate: updatable->onPostUpdate(); break;

		case UpdateEvent_physicsUpdate: 
			updatable->onPhysicsUpdate(); 
			break;

		case UpdateEvent_render:
			updatable->onRender();
			break;
		}
	}
}

void wv::World::updateComponentChanges() 
{
	for ( auto& change : m_componentChangeQueue )
	{
		std::bitset<256> oldBitmask{};
		std::bitset<256> newBitmask{};

		// get old archetype, if any
		if ( Archetype* oldArchetype = change.entity->archetype )
		{
			oldBitmask = oldArchetype->m_bitmask;
			newBitmask = oldArchetype->m_bitmask;
		}

		if ( change.type == ComponentChange::ComponentChangeType_add )
		{
			newBitmask[ change.componentTypeIndex ] = true;

			// add first, notify after
			change.callback();
			checkComponentAddChanges( oldBitmask, newBitmask, change.entity );
		}
		else if ( change.type == ComponentChange::ComponentChangeType_remove )
		{
			newBitmask[ change.componentTypeIndex ] = false;

			// notify first, remove after
			checkComponentRemoveChanges( oldBitmask, newBitmask, change.entity );
			change.callback();
		}
		else if ( change.type == ComponentChange::ComponentChangeType_removeAll )
		{
			newBitmask = {};

			// notify first, remove after
			checkComponentRemoveChanges( oldBitmask, newBitmask, change.entity );
			change.callback();
		}
	}

	if ( m_componentChangeQueue.size() > 0 )
		m_ecsEngine->updateSystemsArchetypes();
	
	m_componentChangeQueue.clear();
}

void wv::World::checkComponentAddChanges( std::bitset<256> _oldBitmask, std::bitset<256> _newBitmask, Entity* _entity )
{
	// find systems that previously didn't match
	std::vector<ISystem*> systemsToCheck;
	for ( auto& [k, v] : m_ecsEngine->m_systemIndexMap )
	{
		if ( !v->matchesBitmask( _oldBitmask ) )
			systemsToCheck.push_back( v );
	}

	for ( auto* s : systemsToCheck )
	{
		// if a system that previously didn't match but now does, it needs to get notified
		if ( s->matchesBitmask( _newBitmask ) )
			s->onComponentAdded( _entity->archetype, _entity->archetype->getEntityIndex( _entity ) );

	}
}

void wv::World::checkComponentRemoveChanges( std::bitset<256> _oldBitmask, std::bitset<256> _newBitmask, Entity* _entity )
{
	// find systems that previously matched
	std::vector<ISystem*> systemsToCheck;
	for ( auto& [k, v] : m_ecsEngine->m_systemIndexMap )
	{
		if ( v->matchesBitmask( _oldBitmask ) )
			systemsToCheck.push_back( v );
	}

	for ( auto* s : systemsToCheck )
	{
		// if a system that previously matched but no longer does, it needs to get notified
		if ( !s->matchesBitmask( _newBitmask ) )
			s->onComponentRemoved( _entity->archetype, _entity->archetype->getEntityIndex( _entity ) );
	}
}
