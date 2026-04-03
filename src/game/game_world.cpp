#include "game_world.h"

#include <wv/entity/ecs.h>
#include <wv/entity/entity.h>

#include <wv/components/mesh_component.h>
#include <wv/components/camera_component.h>
#include <wv/components/orbit_controller_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include <wv/input/input_system.h>


struct PlayerComponent
{

};


class PlayerSystem : public wv::ISystem
{
public:
	virtual void configure( wv::ArchetypeConfig& _config ) override {
		_config.addComponentType<wv::RigidBodyComponent>();
		_config.addComponentType<PlayerComponent>();
	}

	virtual void onInitialize() override
	{
		m_moveActionID = updateContext->inputSystem->getActionGroup( "Player" )->getAxisActionID( "Move" );
		m_jumpActionID = updateContext->inputSystem->getActionGroup( "Player" )->getTriggerActionID( "Jump" );
	}

	virtual void onUpdate() override
	{
		for ( auto& ae : updateContext->actionEventQueue )
		{
			if ( ae.actionID == m_moveActionID )
				m_move = ae.action.axis->getValue();

			if ( ae.actionID == m_jumpActionID && ae.action.trigger->getValue() )
				jump = true;
		}

		if ( m_move.length() > 1.0f )
			m_move.normalize();
	}

	virtual void onPhysicsUpdate() override 
	{
		for ( wv::Archetype* archetype : getArchetypes() )
		{
			auto& rigidbodies = archetype->getComponents<wv::RigidBodyComponent>();
			auto& entities = archetype->getEntities();
			for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
			{
				rigidbodies[ i ].linearVelocity.x =  m_move.x * 2.0f;
				rigidbodies[ i ].linearVelocity.z = -m_move.y * 2.0f;

				if( jump )
					rigidbodies[ i ].linearVelocity.y += 5.0f;

				rigidbodies[ i ].rotation = { 0.0f, 0.0f, 0.0f };
			}
		}

		jump = false;
	}

	bool jump = false;
	wv::Vector2f m_move = {};
	wv::ActionID m_moveActionID;
	wv::ActionID m_jumpActionID;
};

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
	registerComponentType<PlayerComponent>();
	addSystem<PlayerSystem>();

	{
		wv::Entity* entity = createEntity( "Ball" );
		entity->getTransform().position = { -3.0f, 2.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_sphere } );
		addComponent<wv::RigidBodyComponent>( entity, {} );
		addComponent<wv::MeshComponent>( entity, { .assetPath = "meshes/SM_MaterialSphere.glb" } );
	}
	
	{
		wv::Entity* entity = createEntity( "Cube" );
		entity->getTransform().position = { 3.0f, 2.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_box } );
		addComponent<wv::RigidBodyComponent>( entity, {} );
		addComponent<wv::MeshComponent>( entity, { .assetPath = "meshes/SM_MaterialCube.glb" } );
	}
	
	{
		wv::Entity* entity = createEntity( "Player" );
		entity->getTransform().position = { 0.0f, 1.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_cylinder, .cylinderHeight = 1.0f } );
		addComponent<wv::RigidBodyComponent>( entity, {} );
		addComponent<wv::MeshComponent>( entity, { .assetPath = "meshes/SM_MaterialCylinder.glb" } );
		addComponent<PlayerComponent>( entity, { } );
		addComponent<wv::CameraComponent>( entity, { .active = false } );
	}

	{
		wv::Entity* entity = createEntity( "Orbit Camera" );
		
		addComponent<wv::CameraComponent>( entity, { .active = true } );
		addComponent<wv::OrbitControllerComponent>( entity, { .orbitDistance = 10.0f } );
	}
	
}
