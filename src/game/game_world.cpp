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

struct TrackSegment
{
	wv::Vector3f start;
	wv::Vector3f end;

	float length() const {
		return ( end - start ).length();
	}
};

struct Track
{
	void addTrack( const wv::Vector3f& _position ) 
	{
		if ( m_track.empty() )
			m_track.push_back( { {}, _position } );
		else
			m_track.push_back( { m_track.back().end, _position } );
	}

	wv::Vector3f getPositionAt( double _trackPosition )
	{
		size_t trackIndex = findTrackIndex( _trackPosition );
		if ( trackIndex == -1 )
		{
			_trackPosition = 0;
			trackIndex = 0;
		}

		double prevLength = 0.0;
		for ( size_t i = 0; i < trackIndex; i++ )
			prevLength += m_track[ i ].length();

		double relativePos = _trackPosition - prevLength;

		TrackSegment segment = m_track[ trackIndex ];

		return wv::Math::lerp(
			segment.start,
			segment.end,
			relativePos / segment.length()
		);
	}

	int findTrackIndex( double _position ) {
		double currentLength = 0.0;

		for ( size_t i = 0; i < m_track.size(); i++ )
		{
			double nextLength = currentLength + m_track[ i ].length();

			if ( _position <= nextLength )
				return i;

			currentLength = nextLength;
		}

		return -1;
	}

	bool isPositionInsideTrack( double _position ) {
		if ( _position < 0 )
			return false;
		
		if ( findTrackIndex( _position ) == -1 )
			return false;

		return true;
	}

	std::vector<TrackSegment> m_track = {};
};

class PlayerTrainSystem : public wv::IEntitySystem
{
	WV_REFLECT_TYPE( PlayerTrainSystem, wv::IEntitySystem )
public:
	PlayerTrainSystem() = default;
	~PlayerTrainSystem() { }

	void setTrack( const Track& _track ) { m_track = _track; }
	void addCart( wv::Entity* _cart ) { m_carts.push_back( _cart ); }

protected:
	virtual void update( wv::WorldUpdateContext& _ctx ) override { 
		m_trackPos += _ctx.deltaTime * 10.0;

		if ( !m_track.isPositionInsideTrack( m_trackPos ) )
			m_trackPos = 0.0;

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
	Track m_track{};
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

	//sector->addEntity( createMeshEntity( "meshes/SM_MoonRayWidget.glb", { -6.0f, -1.6f, 0.0f } ) );
	//sector->addEntity( createMeshEntity( "meshes/main_sponza/NewSponza_Main_glTF_003.gltf", { 0.0f, -2.0f, 0.0f } ) );
	
	//sector->addEntity( createMeshEntity( "meshes/engine_basic.glb", { 0.0f, -1.0f, 0.0f } ) );

	Track track{};
	
	for ( size_t i = 0; i < 600; i++ )
	{
		float fi = (float)i;
		track.addTrack( { 
			std::sin( fi / 4.0f ) * 3.0f, 
			std::sin( fi / 5.0f ) * 2.0f,
			fi * 3.0f
			} );
	}
	
	{
		wv::Entity* player = WV_NEW( wv::Entity );
		sector->addEntity( player );

		wv::MeshComponent* meshComponent = player->createComponent<wv::MeshComponent>();
		meshComponent->setFilePath( "meshes/engine_basic.glb" );

		auto trainsystem = player->createSystem<PlayerTrainSystem>();
		trainsystem->setTrack( track );

		player->getTransform().setPosition( { 0.0f, -1.0f, 0.0f } );

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

		{
			wv::Entity* camera = WV_NEW( wv::Entity );

			camera->createComponent<wv::PlayerInputComponent>()->setPlayerIndex( 0 );
			camera->createComponent<wv::OrbitCameraComponent>();

			camera->createSystem<CameraFollowSystem>()->setFollow( player );
			sector->addEntity( camera );
		}

	}

	double trackLength = 0.5;
	while ( track.isPositionInsideTrack( trackLength ) )
	{
		wv::Vector3 pos  = track.getPositionAt( trackLength );
		wv::Vector3 posBack = track.getPositionAt( trackLength - 0.5 );

		wv::Entity* trackEntity = createMeshEntity( "meshes/SM_Track.glb", pos );
		trackEntity->getTransform().rotation = ( pos - posBack ).directionToEuler();

		sector->addEntity( trackEntity );

		trackLength += 1.0;
	}

	addSector( sector );

}
