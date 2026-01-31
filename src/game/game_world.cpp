#include "game_world.h"

#include <wv/camera/components/orbit_camera_component.h>
#include <wv/camera/systems/camera_manager_system.h>
#include <wv/entity/world_sector.h>
#include <wv/input/components/player_input_component.h>
#include <wv/rendering/systems/render_world_system.h>
#include <wv/rendering/components/mesh_component.h>
#include <wv/input/input_system.h>

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

void GameWorld::onSceneCreate()
{
	createWorldSystem<wv::RenderWorldSystem>();
	createWorldSystem<wv::CameraManagerSystem>();
	// PlayerInputSystem* playerInputSystem = m_world->createWorldSystem<PlayerInputSystem>();
	// playerInputSystem->setSelectionMode( PlayerInputSystem::SelectionMode::ANY_TRIGGER_ACTION );

	wv::Entity* cameraEntity = WV_NEW( wv::Entity );
	cameraEntity->createComponent<wv::PlayerInputComponent>()->setPlayerIndex( 0 );
	cameraEntity->createComponent<wv::OrbitCameraComponent>();

	cameraEntity->getTransform().setPosition( { 0, 10.0f, 10.0f } );
	cameraEntity->getTransform().setRotation( { -15.0f, 45.0f, 0.0f } );

	wv::Entity* materialEntity = WV_NEW( wv::Entity );
	{
		wv::MeshComponent* meshComponent = materialEntity->createComponent<wv::MeshComponent>();
		meshComponent->setFilePath( "meshes/SM_MoonRayWidget.glb" );
		
		// materialEntity->getTransform().setScale( { 0.5f, 0.5f, 0.5f } );
		materialEntity->getTransform().setPosition( { 0.0f, -1.6f, 0.0f } );
	}

	wv::WorldSector* sector = WV_NEW( wv::WorldSector );
	sector->addEntity( cameraEntity );
	sector->addEntity( materialEntity );
	addSector( sector );

}
