#include "game_world.h"

#include <wv/application.h>

#include <wv/editor/mesh_importer_gltf.h>

#include <wv/entity/ecs.h>
#include <wv/entity/entity.h>

#include <wv/components/mesh_component.h>
#include <wv/components/camera_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include <wv/serialize.h>

#include "player_move_system.h"

void GameWorld::onSetupInput( wv::InputSystem* _inputSystem )
{ 
	wv::ActionGroup* playerActionGroup = _inputSystem->createActionGroup( "Player" );

	playerActionGroup->bindTriggerAction( "Shake", "Controller", wv::CONTROLLER_BUTTON_A );
	playerActionGroup->bindTriggerAction( "Shake", "Keyboard", wv::SCANCODE_SPACE );
	
	playerActionGroup->bindTriggerAction( "DebugMouseLock", "Keyboard", wv::SCANCODE_R );

	playerActionGroup->bindAxisAction( "Move", "Controller", wv::AXIS_DIRECTION_ALL, wv::CONTROLLER_JOYSTICK_LEFT );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_NORTH, wv::SCANCODE_W );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_SOUTH, wv::SCANCODE_S );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_EAST, wv::SCANCODE_D );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_WEST, wv::SCANCODE_A );

	playerActionGroup->bindAxisAction( "Look", "Controller", wv::AXIS_DIRECTION_ALL, wv::CONTROLLER_JOYSTICK_RIGHT );
	playerActionGroup->bindAxisAction( "Look", "Mouse", wv::AXIS_DIRECTION_ALL, wv::MOUSE_MOTION_AXIS );

	playerActionGroup->enable();
}

void to_json( nlohmann::json& _json, const PlayerMoveComponent& _comp ) { 
	_json = nlohmann::json{
		{ "cameraSensitivity",  _comp.cameraSensitivity },
		{ "cameraHeight",       _comp.cameraHeight },
		{ "cameraShakeDecay",   _comp.cameraShakeDecay },
		{ "smoothAcceleration", _comp.smoothAcceleration },
		{ "acceleration",       _comp.acceleration },
		{ "moveSpeed",          _comp.moveSpeed },
		{ "damping",            _comp.damping },
		{ "viewBobbing",        _comp.viewBobbing },
		{ "viewBobbingSpeed",   _comp.viewBobbingSpeed },
		{ "resetBobPosition",   _comp.resetBobPosition },
		{ "viewRotting",        _comp.viewRotting },
		{ "viewRotFrequency",   _comp.viewRotFrequency },
		{ "viewRotOffset",      _comp.viewRotOffset }
	};
}

void GameWorld::onSceneCreate()
{
	registerComponentType<PlayerMoveComponent>();
	
	m_serializer->addComponentFunction<PlayerMoveComponent>();

	addSystem<PlayerMoveSystem>();

	{
		wv::Entity* entity = createEntity( "Ball" );
		entity->getTransform().position = { -3.0f, 2.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_sphere } );
		addComponent<wv::RigidBodyComponent>( entity, {} );
		addComponent<wv::MeshComponent>( 
			entity, 
			wv::MeshComponent{ 
				.meshAsset = wv::MeshAsset::get( "meshes/SM_Sphere.wvb" ),
				.materials = { wv::MaterialAsset::get( "materials/default_unlit.wvmt" ) }
			} 
		);
	}
	
	{
		wv::Entity* entity = createEntity( "Cube" );
		entity->getTransform().position = { 3.0f, 2.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_box } );
		addComponent<wv::RigidBodyComponent>( entity, {} );
		addComponent<wv::MeshComponent>( entity, { .meshAsset = wv::MeshAsset::get( "meshes/SM_Cube.wvb" ) } );
	}

	{
		wv::Entity* entity = createEntity( "Floor" );
		entity->getTransform().position = { 0.0f, -5.0f, 0.0f };
		entity->getTransform().scale = { 50.0f, 0.5f, 50.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_box,.boxSize = { 50.0f, 0.5f, 50.0f } } );
		addComponent<wv::RigidBodyComponent>( entity, wv::RigidBodyComponent{ .bodyType = wv::BodyType_Static } );
		addComponent<wv::MeshComponent>( entity, { .meshAsset = wv::MeshAsset::get( "meshes/SM_Cube.wvb" ) } );
	}
	
	{
		wv::MeshImporterGLTF importer(
			wv::getApp()->getMeshManager(),
			wv::getApp()->getMaterialManager(),
			wv::getApp()->getTextureManager()
		);

		importer.load( "meshes/SM_Tofumotive.glb" );

		wv::Entity* entity = createEntity( "Train" );
		entity->getTransform().position = { 0.0f, -4.5f, 10.0f };
		addComponent<wv::MeshComponent>( entity, { .meshAsset = importer.getMesh(), .materials = importer.getMaterials() });
	}
	
	{
		// Camera

		wv::Entity* camera = createEntity( "PlayerCamera" );
		addComponent<wv::CameraComponent>( camera, { .active = true } );
		
		// Player

		wv::Entity* player = createEntity( "Player" );
		player->getTransform().position = { 0.0f, 1.0f, 0.0f };

		addComponent<wv::ColliderComponent>( player, wv::ColliderComponent{ .shape = wv::ColliderShape_cylinder, .cylinderHeight = 2.0f, .radius = 0.25f } );
		addComponent<wv::RigidBodyComponent>( 
			player,
			{ 
				.mass = 1.0f, 
				.lockRotationAxis{ true, true, true } 
			} 
		);

		addComponent<PlayerMoveComponent>( player, { .cameraEntity = camera } );
	
		// I don't like this
		player->getTransform().addChild( &camera->getTransform() );
	}
	
}
