#include "world.h"

#include <wv/entity/ecs.h>

#include <wv/components/camera_component.h>
#include <wv/components/mesh_component.h>
#include <wv/components/orbit_controller_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include <wv/updatable.h>
#include <wv/application.h>

#include <wv/filesystem/file_system.h>

#include <wv/serialize.h>

#include <fstream>

namespace wv {

void to_json( nlohmann::json& _json, const MeshComponent& _comp ) {
	std::vector<std::string> materials;

	for ( size_t i = 0; i < _comp.materials.size(); i++ )
		materials.push_back( _comp.materials[ i ]->path.string() );
	
	_json = nlohmann::json{
		{ "mesh", _comp.meshAsset->getPath().string() },
		{ "materials", materials }
	};
}

void to_json( nlohmann::json& _json, const CameraComponent& _comp ) {
	_json = nlohmann::json{
		{ "projectionType", _comp.projectionType },
		{ "fov", _comp.fov },
		{ "clipNear", _comp.clipNear },
		{ "clipFar", _comp.clipFar },
		{ "orthoScale", _comp.orthoScale },
		{ "aspect", _comp.aspect }
	};
}

void to_json( nlohmann::json& _json, const ColliderComponent& _comp ) {
	_json = nlohmann::json{
		{ "shape", _comp.shape }
	};

	switch ( _comp.shape )
	{
	case ColliderShape_box: 
		_json[ "boxSize" ] = _comp.boxSize; 
		break;
	case ColliderShape_cylinder: 
		_json[ "boxSize" ] = _comp.boxSize; 
		_json[ "cylinderHeight" ] = _comp.cylinderHeight;
		_json[ "radius" ] = _comp.radius;
		break;
	case ColliderShape_sphere: 
		_json[ "boxSize" ] = _comp.boxSize; 
		_json[ "radius" ] = _comp.radius;
		break;
	}
}

void to_json( nlohmann::json& _json, const OrbitControllerComponent& _comp ) {
	_json = nlohmann::json{
		{ "orbitDistance", _comp.orbitDistance },
	};
}

void to_json( nlohmann::json& _json, const RigidBodyComponent& _comp ) {
	_json = nlohmann::json{
		{ "bodyType", _comp.bodyType },
		{ "mass", _comp.mass },
		{ "linearDamping", _comp.linearDamping },
		{ "lockPositionAxis", _comp.lockPositionAxis },
		{ "lockRotationAxis", _comp.lockRotationAxis }
	};
}

}

wv::World::World()
{ 
	m_ecsEngine  = WV_NEW( ECSEngine );
	m_serializer = WV_NEW( WorldSerializer, m_ecsEngine );

	// register order determines internal ID
	registerComponentType<CameraComponent>();
	registerComponentType<ColliderComponent>();
	registerComponentType<MeshComponent>();
	registerComponentType<OrbitControllerComponent>();
	registerComponentType<RigidBodyComponent>();

	m_serializer->addComponentFunction<CameraComponent>();
	m_serializer->addComponentFunction<ColliderComponent>();
	m_serializer->addComponentFunction<MeshComponent>();
	m_serializer->addComponentFunction<OrbitControllerComponent>();
	m_serializer->addComponentFunction<RigidBodyComponent>();
}

wv::World::~World()
{
	destroyAllEntities();

	WV_FREE( m_serializer );
	WV_FREE( m_ecsEngine );

	if ( m_viewport )
		WV_FREE( m_viewport );	
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

void wv::World::loadWorld( const std::filesystem::path& _path )
{
}

void wv::World::saveWorld( const std::filesystem::path& _path )
{
	nlohmann::ordered_json json{};
	json[ "name" ] = "Test World";

	json[ "components" ] = m_serializer->serializeComponents();

	std::filesystem::path fullpath = getApp()->getFileSystem()->getMountedPath( _path );
	std::ofstream stream{ fullpath };
	stream << json.dump( 2 );
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
