#include "player_move_system.h"

#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/components/rigidbody_component.h>
#include <wv/input/input_system.h>

void PlayerMoveSystem::configure( wv::ArchetypeConfig& _config )
{
	_config.addComponentType<wv::RigidBodyComponent>();
	_config.addComponentType<PlayerMoveComponent>();
}

void PlayerMoveSystem::onInitialize()
{
	m_moveActionID = updateContext->inputSystem->getActionGroup( "Player" )->getAxisActionID( "Move" );
	m_jumpActionID = updateContext->inputSystem->getActionGroup( "Player" )->getTriggerActionID( "Jump" );
}

void PlayerMoveSystem::onUpdate()
{
	for ( auto& ae : updateContext->actionEventQueue )
	{
		if ( ae.actionID == m_moveActionID )
			m_moveInput = ae.action.axis->getValue();

		if ( ae.actionID == m_jumpActionID && ae.action.trigger->getValue() )
			jump = true;
	}

	if ( m_moveInput.length() > 1.0f )
		m_moveInput.normalize();
}

void PlayerMoveSystem::onPhysicsUpdate()
{
	for ( wv::Archetype* archetype : getArchetypes() )
	{
		auto& entities = archetype->getEntities();
		auto& rigidbodies = archetype->getComponents<wv::RigidBodyComponent>();
		auto& playerComponents = archetype->getComponents<PlayerMoveComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			wv::RigidBodyComponent& rb = rigidbodies[ i ];
			PlayerMoveComponent& pc = playerComponents[ i ];
			updateMove( entities[ i ], rb, pc );
		}
	}

	jump = false;
}

void PlayerMoveSystem::updateMove( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc )
{

	wv::Vector3f forward = _pc.cameraEntity->getTransform().forward();
	wv::Vector3f right = _pc.cameraEntity->getTransform().right();

	const float force = 50.0f;
	const float jumpforce = 5.0f * _rb.mass;
	wv::Vector3 moveDirection = forward * m_moveInput.y + right * m_moveInput.x;

	if ( moveDirection.length() > 0.0f )
	{
		moveDirection.y = 0.0f;
		moveDirection.normalize();

		_rb.addForce(
			moveDirection * force,
			wv::ForceType_force
		);
	}

	// Clamp speed
	{
		wv::Vector3f vel = _rb.linearVelocity;
		vel.y = 0.0f;

		if ( vel.length() > 3.0f )
		{
			vel.normalize( 3.0f );
			_rb.linearVelocity.x = vel.x;
			_rb.linearVelocity.z = vel.z;
		}
	}

	// Move damping
	if ( m_moveInput.length() < 0.0001f )
	{
		wv::Vector3f vel = _rb.linearVelocity;
		vel.y = 0.0f;

		_rb.addForce( -vel * 10.0f, wv::ForceType_acceleration );
	}

	// Jump
	if ( jump )
		_rb.addForce( { 0.0f, jumpforce, 0.0f }, wv::ForceType_impulse );
}


