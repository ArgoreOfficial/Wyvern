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
				wv::RigidBodyComponent& rb = rigidbodies[ i ];

				const float force     = 200.0f;
				const float jumpforce = 50.0f;

				rb.addForce(
					{ m_move.x * force, 0.0f, -m_move.y * force },
					wv::ForceType_force
				);
				
				// Clamp speed
				{ 
					wv::Vector3f vel = rb.linearVelocity;
					vel.y = 0.0f;

					if ( vel.length() > 3.0f )
					{
						vel.normalize( 3.0f );
						rb.linearVelocity.x = vel.x;
						rb.linearVelocity.z = vel.z;
					}
				}
				
				// Move damping
				if( m_move.length() < 0.0001f )
				{
					wv::Vector3f vel = rb.linearVelocity;
					vel.y = 0.0f;

					rb.addForce( -vel * 10.0f, wv::ForceType_acceleration );
				}

				// Jump
				if ( jump )
					rb.addForce( { 0.0f, jumpforce, 0.0f }, wv::ForceType_impulse );
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
		wv::Entity* entity = createEntity( "Player" );
		entity->getTransform().position = { 0.0f, 1.0f, 0.0f };

		addComponent<wv::ColliderComponent>( entity, wv::ColliderComponent{ .shape = wv::ColliderShape_cylinder, .cylinderHeight = 1.0f } );
		addComponent<wv::RigidBodyComponent>( 
			entity, 
			{ 
				.mass = 10.0f, 
				.lockRotationAxis{ true, true, true } 
			} 
		);

		addComponent<wv::MeshComponent>( entity, { .assetPath = "meshes/SM_Cylinder.glb" } );
		
		addComponent<wv::CameraComponent>( entity, { .active = false } );
		
		addComponent<PlayerComponent>( entity, { } );
	}

	{
		wv::Entity* entity = createEntity( "Orbit Camera" );
		
		addComponent<wv::CameraComponent>( entity, { .active = true } );
		addComponent<wv::OrbitControllerComponent>( entity, { .orbitDistance = 10.0f } );
	}
	
}
