#include "game_world.h"

#include <wv/entity/ecs.h>
#include <wv/entity/entity.h>

#include <wv/components/mesh_component.h>
#include <wv/components/camera_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include "player_move_system.h"
#include <wv/components/orbit_controller_component.h>

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
	registerComponentType<PlayerMoveComponent>();
	
	addSystem<PlayerMoveSystem>();

	{
		wv::Entity* entity = createEntity( "Ball" );
		entity->getTransform().position = { -3.0f, 2.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_sphere } );
		addComponent<wv::RigidBodyComponent>( entity, {} );
		addComponent<wv::MeshComponent>( entity, { .assetPath = "meshes/SM_Sphere.glb" } );
	}
	
	{
		wv::Entity* entity = createEntity( "Cube" );
		entity->getTransform().position = { 3.0f, 2.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_box } );
		addComponent<wv::RigidBodyComponent>( entity, {} );
		addComponent<wv::MeshComponent>( entity, { .assetPath = "meshes/SM_Cube.glb" } );
	}
	
	{
		// Camera

		wv::Entity* camera = createEntity( "PlayerCamera" );
		addComponent<wv::CameraComponent>( camera, { .active = true } );
		addComponent<wv::OrbitControllerComponent>( camera, { .orbitDistance = 10.0f } );

		// Player

		wv::Entity* player = createEntity( "Player" );
		player->getTransform().position = { 0.0f, 1.0f, 0.0f };

		addComponent<wv::ColliderComponent>( player, wv::ColliderComponent{ .shape = wv::ColliderShape_cylinder, .cylinderHeight = 1.0f } );
		addComponent<wv::RigidBodyComponent>( 
			player,
			{ 
				.mass = 1.0f, 
				.lockRotationAxis{ true, true, true } 
			} 
		);

		addComponent<wv::MeshComponent>( player, { .assetPath = "meshes/SM_Cylinder.glb" } );
		addComponent<PlayerMoveComponent>( player, { .cameraEntity = camera } );
	
		// I don't like this
		player->getTransform().addChild( &camera->getTransform() );
	}
	
}
