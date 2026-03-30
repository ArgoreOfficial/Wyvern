#include "game_world.h"

#include <wv/entity/ecs.h>
#include <wv/entity/entity.h>

#include <wv/components/mesh_component.h>
#include <wv/components/camera_component.h>

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
		wv::Entity* cube = createEntity( "Cube" );
		cube->getTransform().setPositionRotation( { 0.0f, -1.0f, 0.0f }, { 0.0f, -45.0f, 0.0f } );
		
		wv::MeshImporterGLTF importer = wv::MeshImporterGLTF( m_meshManager, m_materialManager, m_textureManager );
		importer.load( "meshes/SM_Tofumotive.glb", {} );

		wv::MeshComponent meshComponent{};
		meshComponent.materials = importer.getMaterials();
		meshComponent.meshAsset = importer.getMesh();

		m_ecsEngine->addComponent<wv::MeshComponent>( cube, meshComponent );
	}

	{
		wv::Entity* camera = createEntity( "Orbit Camera" );
		
		wv::CameraComponent cameraComponent{};
		cameraComponent.active = true;
		cameraComponent.viewVolume = wv::ViewVolume( wv::ViewVolume::kPerspective, 900, 600 );
		m_ecsEngine->addComponent<wv::CameraComponent>( camera, cameraComponent );
	}
	
}
