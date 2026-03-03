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

#include "vtuber_controller.h"

void GameWorld::onSetupInput( wv::InputSystem* _inputSystem )
{ 
	
}

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

	{
		wv::Entity* backdrop = WV_NEW( wv::Entity, "Backdrop" );
		sector->addEntity( backdrop );

		//wv::MeshComponent* meshComponent = backdrop->createComponent<wv::MeshComponent>();
		//meshComponent->setFilePath( "meshes/SM_Tatiana.glb" );

		backdrop->getTransform().position = { -97.5f,  -32.0f, -57.5f };
		backdrop->getTransform().rotation = {   0.0f, -105.0f,   0.0f };
	}

	{
		wv::Entity* player = WV_NEW( wv::Entity, "Player" );
		sector->addEntity( player );

		wv::MeshImportOptions importOptions{};
		importOptions.transform = wv::Math::translate( importOptions.transform, { 0.0f, 0.0f, 0.0f } );

		wv::MeshComponent* meshComponent = player->createComponent<wv::MeshComponent>();
		meshComponent->setFilePath( "meshes/SM_Frog.glb" );
		meshComponent->importOptions = importOptions;

		player->createSystem<VtuberControllerSystem>();
	}

	{
		wv::Entity* camera = WV_NEW( wv::Entity, "Orbit Camera" );
		camera->getTransform().position.y = 0.3f;

		camera->createComponent<wv::PlayerInputComponent>()->setPlayerIndex( 0 );
		camera->createComponent<wv::OrbitCameraComponent>();
		sector->addEntity( camera );
	}

	addSector( sector );

}
