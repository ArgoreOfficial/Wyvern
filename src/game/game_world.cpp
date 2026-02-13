#include "game_world.h"

#include <wv/application.h>
#include <wv/thread/task_system.h>
#include <wv/thread/thread.h>

#include <wv/camera/components/orbit_camera_component.h>
#include <wv/camera/systems/camera_manager_system.h>
#include <wv/entity/world_sector.h>
#include <wv/input/components/player_input_component.h>
#include <wv/rendering/systems/render_world_system.h>
#include <wv/rendering/components/mesh_component.h>
#include <wv/input/input_system.h>

#include <wv/entity/entity_system.h>
#include <wv/math/math.h>

#include "track.h"
#include "track_vehicle_system.h"

void GameWorld::onSetupInput( wv::InputSystem* _inputSystem )
{ 
	wv::ActionGroup* playerActionGroup = _inputSystem->createActionGroup( "Player" );

	playerActionGroup->bindTriggerAction( "Jump", "Controller", wv::CONTROLLER_BUTTON_A );
	playerActionGroup->bindTriggerAction( "Jump", "Keyboard", wv::SCANCODE_SPACE );

	playerActionGroup->bindAxisAction( "Move", "Controller", wv::AXIS_DIRECTION_ALL, wv::CONTROLLER_JOYSTICK_LEFT );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_NORTH, wv::SCANCODE_W );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_SOUTH, wv::SCANCODE_S );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_EAST, wv::SCANCODE_D );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_WEST, wv::SCANCODE_A );

	playerActionGroup->bindAxisAction( "Throttle", "Keyboard", wv::AXIS_DIRECTION_NORTH, wv::SCANCODE_W );
	playerActionGroup->bindAxisAction( "Throttle", "Keyboard", wv::AXIS_DIRECTION_SOUTH, wv::SCANCODE_S );

	playerActionGroup->enable();
}

wv::Entity* createMeshEntity( const std::filesystem::path _path, const wv::Vector3f& _position ) {
	wv::Entity* entity = WV_NEW( wv::Entity );
	
	wv::MeshComponent* meshComponent = entity->createComponent<wv::MeshComponent>();
	meshComponent->setFilePath( _path );

	// materialEntity->getTransform().setScale( { 0.5f, 0.5f, 0.5f } );
	entity->getTransform().setPosition( _position );
	
	return entity;
}

class PlayerTrainSystem : public wv::IEntitySystem
{
	WV_REFLECT_TYPE( PlayerTrainSystem, wv::IEntitySystem )
public:
	PlayerTrainSystem() = default;
	~PlayerTrainSystem() {}

protected:

	virtual void initialize() 
	{ 

	}

	virtual void shutdown() { }

	virtual void registerComponent( wv::IEntityComponent* _component ) 
	{ 
		if ( TrackEngineComponent* comp = wv::tryCast<TrackEngineComponent>( _component ) )
		{
			WV_ASSERT( m_engineComponent == nullptr );
			m_engineComponent = comp;
		}
	};

	virtual void unregisterComponent( wv::IEntityComponent* _component ) 
	{
		if ( TrackEngineComponent* comp = wv::tryCast<TrackEngineComponent>( _component ) )
		{
			if( comp == m_engineComponent )
				m_engineComponent = nullptr;
		}
	};

	virtual void update( wv::WorldUpdateContext& _ctx ) override 
	{ 
		if ( !m_engineComponent )
			return;

		wv::ActionID throttle = _ctx.inputSystem->getActionGroup( "Player" )->getAxisActionID( "Throttle" );

		for ( auto& ev : _ctx.actionEventQueue )
		{
			if ( ev.actionID == throttle )
			{
				float v = ev.action.axis->getValue().y;
				wv::Debug::Print( "Throttle: %f\n", v );
				m_engineComponent->setThrottle( v );
			}
		}
	}

	TrackEngineComponent* m_engineComponent{ nullptr };
};

class CameraFollowSystem : public wv::IEntitySystem
{
	WV_REFLECT_TYPE( CameraFollowSystem, wv::IEntitySystem )
public:
	CameraFollowSystem() = default;
	~CameraFollowSystem() { }

	void setFollow( wv::Entity* _entity ) { m_followEntity = _entity; }

protected:
	virtual void update( wv::WorldUpdateContext& _ctx ) override {
		if ( m_followEntity == nullptr )
			return;

		m_entity->getTransform().position = m_followEntity->getTransform().position;
		m_entity->getTransform().update( nullptr );
	}

	wv::Entity* m_followEntity = nullptr;
};

void GameWorld::onSceneCreate()
{
	createWorldSystem<wv::RenderWorldSystem>();
	createWorldSystem<wv::CameraManagerSystem>();
	// PlayerInputSystem* playerInputSystem = m_world->createWorldSystem<PlayerInputSystem>();
	// playerInputSystem->setSelectionMode( PlayerInputSystem::SelectionMode::ANY_TRIGGER_ACTION );

	wv::WorldSector* sector = WV_NEW( wv::WorldSector );

	TrackLength trackLength;
	trackLength.addLineTrack( 50.0f );
	trackLength.addArcTrack( 50.0, 90.0 );
	trackLength.addArcTrack( 50.0, 90.0 );
	trackLength.addLineTrack( 50.0f );
	trackLength.addArcTrack( 50.0, 90.0 );
	trackLength.addArcTrack( 50.0, 90.0 );
	
	TrackVehicleSystem* trackVehicleSys = createWorldSystem<TrackVehicleSystem>();
	trackVehicleSys->addTrackLength( trackLength );
	{
		wv::Entity* notplayer = WV_NEW( wv::Entity );
		sector->addEntity( notplayer );

		wv::MeshComponent* meshComponent = notplayer->createComponent<wv::MeshComponent>();
		meshComponent->setFilePath( "meshes/cart_basic.glb" );

		notplayer->createComponent<TrackEngineComponent>()->m_trackPosition = -8.0f;
	}
		
	{
		wv::Entity* player = WV_NEW( wv::Entity );
		sector->addEntity( player );

		wv::MeshComponent* meshComponent = player->createComponent<wv::MeshComponent>();
		meshComponent->setFilePath( "meshes/engine_basic.glb" );

		player->createComponent<TrackEngineComponent>();
		player->getTransform().setPosition( { 0.0f, -1.0f, 0.0f } );
		
		player->createSystem<PlayerTrainSystem>();

		/*
		for ( size_t i = 0; i < 10; i++ )
		{
			wv::Entity* cart = nullptr;
			if ( wv::Math::randomU32() % 2 == 0 )
				cart = createMeshEntity( "meshes/cart_big.glb", {} );
			else
				cart = createMeshEntity( "meshes/cart_basic.glb", {} );

			trainsystem->addCart( cart );
			sector->addEntity( cart );
		}
		*/

		{
			wv::Entity* camera = WV_NEW( wv::Entity );

			camera->createComponent<wv::PlayerInputComponent>()->setPlayerIndex( 0 );
			camera->createComponent<wv::OrbitCameraComponent>();

			camera->createSystem<CameraFollowSystem>()->setFollow( player );
			sector->addEntity( camera );
		}

	}

	double trackpos = 0.0;
	while ( trackLength.isPositionInsideTrack( trackpos ) )
	{
		wv::Vector3 posBack = trackLength.getPositionAt( trackpos );
		wv::Vector3 pos     = trackLength.getPositionAt( trackpos + 0.5 );

		wv::Entity* trackEntity = createMeshEntity( "meshes/SM_Track.glb", pos );
		trackEntity->getTransform().rotation = ( pos - posBack ).directionToEuler();

		sector->addEntity( trackEntity );

		trackpos += 1.0;
	}
	
	addSector( sector );

}
