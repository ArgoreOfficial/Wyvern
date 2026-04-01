#include "game_world.h"

#include <wv/entity/ecs.h>
#include <wv/entity/entity.h>

#include <wv/components/mesh_component.h>
#include <wv/components/camera_component.h>
#include <wv/components/orbit_controller_component.h>
#include <wv/components/rigidbody_component.h>

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
	

	for ( size_t y = 0; y < 25; y++ )
	{
		for ( size_t x = 0; x < 25; x++ )
		{
			wv::Entity* ball = createEntity( "Ball" );
			ball->getTransform().position = {
				( (float)x - 12.5f ) * 2.0f,
				(float)( x + y ),
				( (float)y - 12.5f ) * 2.0f
			};

			m_ecsEngine->addComponent<wv::MeshComponent>( ball, { .assetPath = "meshes/SM_MaterialSphere.glb" } );
			m_ecsEngine->addComponent<wv::RigidBodyComponent>( ball, {} );
		}
	}

	if( false )
	{
		wv::Entity* cube = createEntity( "Cube" );
		cube->getTransform().setPositionRotation( { 0.0f, -1.0f, 0.0f }, { 0.0f, -45.0f, 0.0f } );
		
		m_ecsEngine->addComponent<wv::MeshComponent>( cube, { .assetPath = "meshes/SM_Tofumotive.glb" } );
	}

	{
		wv::Entity* camera = createEntity( "Orbit Camera" );
		
		wv::CameraComponent cameraComponent{};
		m_ecsEngine->addComponent<wv::CameraComponent>( camera, { .active = true } );
		m_ecsEngine->addComponent<wv::OrbitControllerComponent>( camera, { .orbitDistance = 45.0f } );
	}
	
}
