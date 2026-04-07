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

	m_isWalking = m_moveInput.length() > 0.0f;
}

void PlayerMoveSystem::onPostUpdate()
{

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
			capSpeed( entities[ i ], rb, pc );
		}
	}

	jump = false;
}

void PlayerMoveSystem::updateMove( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc )
{
	wv::Transformf& cameraTransform = _pc.cameraEntity->getTransform();
	
	wv::Vector3 forward = cameraTransform.forward(); 
	forward.y = 0.0f; 
	forward.normalize();
	
	wv::Vector3 right = cameraTransform.right();
	right.y = 0.0f; 
	right.normalize();

	wv::Vector3 moveDirection = 
		forward * m_moveInput.y + 
		right   * m_moveInput.x;

	if ( _pc.smoothAcceleration )
		_rb.addForce( moveDirection * _pc.acceleration * 10.0f, wv::ForceType_force );
	else
	{
		wv::Vector3 targetVelocity = moveDirection * _pc.moveSpeed;
		_rb.linearVelocity = wv::Vector3f( targetVelocity.x, _rb.linearVelocity.y, targetVelocity.z );
	}

	// apply damping if grounded
	/*
	RaycastHit hit;
	isGrounded = Physics.SphereCast( transform.position, GroundedCheckRadius, Vector3.down, out hit, GroundedCheckDistance, groundLayerMask );

	if ( isGrounded )
		_rb.linearDamping = damping;
	else
		_rb.linearDamping = 0.0f;
	*/

	_rb.linearDamping = _pc.damping;
}

void PlayerMoveSystem::capSpeed( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc )
{
	wv::Vector3 flatVelocity = wv::Vector3{ _rb.linearVelocity.x, 0.0f, _rb.linearVelocity.z };
	if ( flatVelocity.length() > _pc.moveSpeed )
	{
		wv::Vector3 limitedVelocity = flatVelocity.normalized() * _pc.moveSpeed;
		_rb.linearVelocity = wv::Vector3{ limitedVelocity.x, _rb.linearVelocity.y, limitedVelocity.z };
	}
}


