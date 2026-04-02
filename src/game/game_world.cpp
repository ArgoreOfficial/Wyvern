#include "game_world.h"

#include <wv/entity/ecs.h>
#include <wv/entity/entity.h>

#include <wv/components/mesh_component.h>
#include <wv/components/camera_component.h>
#include <wv/components/orbit_controller_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

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

	playerActionGroup->bindAxisAction( "Throttle", "Keyboard", wv::AXIS_DIRECTION_NORTH, wv::SCANCODE_W );
	playerActionGroup->bindAxisAction( "Throttle", "Keyboard", wv::AXIS_DIRECTION_SOUTH, wv::SCANCODE_S );

	playerActionGroup->enable();
}

void GameWorld::onSceneCreate()
{
	
	{
		wv::Entity* ball = createEntity( "Ball" );
		ball->getTransform().position = { 0.0f, 4.0f, 0.0f };

		addComponent<wv::ColliderComponent>( ball, wv::ColliderComponent{ .shape = wv::ColliderShape_sphere } );
		addComponent<wv::RigidBodyComponent>( ball, {} );
		addComponent<wv::MeshComponent>( ball, { .assetPath = "meshes/SM_MaterialSphere.glb" } );
	}
	
	{
		wv::Entity* cube = createEntity( "Cube" );
		cube->getTransform().position = { 0.1f, 5.0f, 0.0f };

		addComponent<wv::ColliderComponent>( cube, wv::ColliderComponent{ .shape = wv::ColliderShape_box } );
		addComponent<wv::RigidBodyComponent>( cube, {} );
		addComponent<wv::MeshComponent>( cube, { .assetPath = "meshes/SM_MaterialCube.glb" } );
	}

	{
		wv::Entity* camera = createEntity( "Orbit Camera" );
		
		addComponent<wv::CameraComponent>( camera, { .active = true } );
		addComponent<wv::OrbitControllerComponent>( camera, { .orbitDistance = 15.0f } );
	}
	
}
