#include "world.h"

#include <wv/entity/ecs.h>
#include <wv/editor/editor_object_component.h>

#include <wv/components/camera_component.h>
#include <wv/components/mesh_component.h>
#include <wv/components/orbit_controller_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include <wv/rendering/mesh.h>
#include <wv/rendering/material.h>
#include <wv/rendering/texture.h>

#include <wv/updatable.h>
#include <wv/application.h>

#include <wv/filesystem/file_system.h>

#include <wv/serialize.h>

#include <fstream>

wv::World::World()
{
	m_ecsEngine = WV_NEW( ECSEngine );
	
	// register order determines internal ID
	registerComponentType<CameraComponent>( "CameraComponent" );
	registerComponentType<ColliderComponent>( "ColliderComponent" );
	registerComponentType<MeshComponent>( "MeshComponent" );
	registerComponentType<OrbitControllerComponent>( "OrbitControllerComponent" );
	registerComponentType<RigidBodyComponent>( "RigidBodyComponent" );
}

wv::World::~World()
{
	unload( true );

	WV_FREE( m_ecsEngine );

	if ( m_viewport )
		WV_FREE( m_viewport );
}

void wv::World::unload( bool _destroyPersistent )
{
	if ( m_entities.empty() )
		return;

	for ( Entity* e : m_entities )
	{
		if ( ( !e->getIsPersistent() || _destroyPersistent ) && e->archetype )
			removeAllComponents( e );
	}

	updateComponentChanges();

	std::vector<Entity*> newEntities{};

	for ( Entity* e : m_entities )
	{
		if ( !e->getIsPersistent() || _destroyPersistent )
			WV_FREE( e );
		else
			newEntities.push_back( e );
	}

	m_entities = newEntities;

	m_path = "";
}

wv::Entity* wv::World::createEntity( const std::string& _name )
{
	Entity* e = WV_NEW( Entity, _name );
	
#ifndef WV_PACKAGE
	addComponent<EditorObjectComponent>( e, {} );
#endif

	m_entities.push_back( e );
	return e;
}

void wv::World::destroyEntity( Entity* _entity )
{
	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] != _entity )
			continue;

		m_entities.erase( m_entities.begin() + i );
		m_destroyEntities.push_back( _entity );
		return;
	}
}

void wv::World::load( const std::filesystem::path& _path )
{
	std::filesystem::path fullpath = getApp()->getFileSystem()->getFullPath( _path );
	std::fstream stream{ fullpath };
	if ( !stream )
		return;

	m_path = _path;

	nlohmann::json json = nlohmann::json::parse( stream );

	// parse entities
	/*
	for ( auto& ent : json[ "entities" ] )
	{
		Entity* entity = createEntity( ent[ "name" ] );
		
		entity->m_transform = ent.value<Transform>( "tfm", {} );
		entity->m_ID = ent.value<uint64_t>( "id", wv::Math::randomU64() );
	}

	// parse hierarchy
	for ( auto& ent : json[ "hierarchy" ] )
	{
		Entity* entity = getEntityFromID( ent.value<uint64_t>( "entity", 0 ) );
		if ( !entity )
			continue;

		for ( auto& child : ent[ "children" ] )
			entity->addChild( getEntityFromID( (uint64_t)child ) );
	}

	// parse components
	for ( auto& comps : json[ "components" ] )
	{
		int index = comps[ "index" ];

		m_serializer->deserializeComponents( index, this, comps[ "comps" ] );
	}
	*/

}

void wv::World::save( const std::filesystem::path& _path )
{
	m_path = _path;

	nlohmann::ordered_json json{};
	json[ "name" ] = "Test World";

	std::vector<nlohmann::json> entities{};
	std::vector<nlohmann::json> hierarchy{};
	/*
	for ( Entity* e : m_entities )
	{
		if ( !e->getShouldSerialize() )
			continue;

		entities.push_back(
			nlohmann::json{
				{ "id", (uint64_t)e->m_ID },
				{ "name", e->m_debugName },
				{ "tfm", e->m_transform },
			} );
	}

	for ( Entity* e : m_entities )
	{
		if ( !e->getShouldSerialize() )
			continue;

		std::vector<uint64_t> children;
		
		for ( Entity* child : e->getChildren() )
			children.push_back( child->getID() );
		
		if ( !children.empty() )
		{
			hierarchy.push_back(
				nlohmann::json{
					{ "entity", (uint64_t)e->m_ID },
					{ "children", children }
				} );
		}
	}

	*/
	json[ "entities" ] = entities;
	json[ "hierarchy" ] = hierarchy;
	json[ "components" ] = {};

	std::filesystem::path fullpath = getApp()->getFileSystem()->getMountedPath( _path );
	std::ofstream stream{ fullpath };
	stream << json.dump( 2 );
}

void wv::World::reload( bool _reloadAssets )
{
	std::filesystem::path path = m_path;

	Application* app = getApp();
	std::vector<Ref<MeshAsset>> meshes;
	std::vector<Ref<MaterialAsset>> materials;
	std::vector<Ref<TextureAsset>> textures;
	
	if ( !_reloadAssets )
	{
		meshes = app->getMeshManager()->getManaged();
		materials = app->getMaterialManager()->getManaged();
		textures = app->getTextureManager()->getManaged();
	}
	
	unload( false );
	load( path );
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

void wv::World::insertUpdatable( IUpdatable* _updatable )
{
	if ( m_updatables.contains( _updatable ) )
		return;

	_updatable->m_world = this;
	m_updatables.insert( _updatable );
}

void wv::World::eraseUpdatable( IUpdatable* _updatable )
{
	if ( !m_updatables.contains( _updatable ) )
		return;

	_updatable->m_world = nullptr;
	m_updatables.erase( _updatable );
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

	for ( Entity* entity : m_destroyEntities )
		WV_FREE( entity );
	
	m_destroyEntities.clear();
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
