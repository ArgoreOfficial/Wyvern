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
	~PlayerTrainSystem() { 
		m_track.clear();
	}

	void setTrack( const TrackLength& _track ) { m_track = _track; }
	void addCart( wv::Entity* _cart ) { m_carts.push_back( _cart ); }

protected:
	virtual void update( wv::WorldUpdateContext& _ctx ) override { 
		
		double newTrackPos = m_trackPos + _ctx.deltaTime * 10.0;
		if ( m_track.isPositionInsideTrack( newTrackPos + 2.0 ) )
			m_trackPos = newTrackPos;

		wv::Vector3f frontWheelPos = m_track.getPositionAt( m_trackPos + 1.0 );
		wv::Vector3f backWheelPos  = m_track.getPositionAt( m_trackPos - 1.0 );
		
		m_entity->getTransform().position = ( frontWheelPos + backWheelPos ) / 2.0;
		m_entity->getTransform().rotation = ( frontWheelPos - backWheelPos ).normalized().directionToEuler();
		m_entity->getTransform().update( nullptr );

		updateCarts( _ctx );

	}

	void updateCarts( wv::WorldUpdateContext& _ctx ) 
	{
		for ( size_t i = 0; i < m_carts.size(); i++ )
		{
			double fpos = m_trackPos - ( 7.8 * i ) - 5.0;
			
			wv::Vector3f frontWheelPos = m_track.getPositionAt( wv::Math::max( 0.0, fpos ) );
			wv::Vector3f backWheelPos  = m_track.getPositionAt( wv::Math::max( 0.0, fpos - 5.0 ) );

			m_carts[ i ]->getTransform().position = ( frontWheelPos + backWheelPos ) / 2;
			m_carts[ i ]->getTransform().rotation = ( frontWheelPos - backWheelPos ).normalized().directionToEuler();
			m_carts[ i ]->getTransform().update( nullptr );
		}
	}

	double m_trackPos = 0.0;
	TrackLength m_track{};
	std::vector<wv::Entity*> m_carts;
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
		wv::Entity* player = WV_NEW( wv::Entity );
		sector->addEntity( player );

		wv::MeshComponent* meshComponent = player->createComponent<wv::MeshComponent>();
		meshComponent->setFilePath( "meshes/engine_basic.glb" );

		player->createComponent<TrackEngineComponent>();
		player->getTransform().setPosition( { 0.0f, -1.0f, 0.0f } );
		
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
