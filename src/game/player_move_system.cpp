#include "player_move_system.h"

#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/components/rigidbody_component.h>
#include <wv/input/input_system.h>

#include <wv/systems/physics_system.h>

#include <wv/application.h>

void PlayerMoveSystem::configure( wv::ArchetypeConfig& _config )
{
	m_debugName = "PlayerMoveSystem";

	_config.addComponentType<wv::RigidBodyComponent>();
	_config.addComponentType<PlayerMoveComponent>();
}

void PlayerMoveSystem::onInitialize()
{
	playerActionGroup = updateContext->inputSystem->getActionGroup( "Player" );
	
	m_moveActionID  = playerActionGroup->getAxisActionID( "Move" );
	m_shakeActionID = playerActionGroup->getTriggerActionID( "Shake" );

	m_mouseLockActionID = playerActionGroup->getTriggerActionID( "DebugMouseLock" );
}

void PlayerMoveSystem::onUpdate()
{
	// special case because we are dealing with mouse *and* controller look input
	
	m_lookInput = playerActionGroup->getAxisValue( -1, "Look" );
	float cameraShake = 0.0f;

	for ( auto& ae : updateContext->actionEventQueue )
	{
		if ( ae.actionID == m_moveActionID )
		{
			m_moveInput = ae.action.axis->getValue();
		}
		else if ( ae.actionID == m_mouseLockActionID && ae.action.trigger->getValue() )
		{
			wv::getApp()->setCursorLock( !isLocked );
			isLocked = !isLocked;
		}
		else if ( ae.actionID == m_shakeActionID && ae.action.trigger->getValue() )
		{
			cameraShake = 1.0f;
		}
	}

	// if the mouse isn't locked, skip input
	if ( !isLocked )
	{
		m_moveInput = {};
		m_lookInput = {};
	}

	if ( m_moveInput.length() > 1.0f )
		m_moveInput.normalize();

	m_isWalking = m_moveInput.length() > 0.0f;


	// 

	for ( wv::Archetype* archetype : getArchetypes() )
	{
		auto& entities = archetype->getEntities();
		auto& rigidbodies = archetype->getComponents<wv::RigidBodyComponent>();
		auto& playerComponents = archetype->getComponents<PlayerMoveComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			auto& rb = rigidbodies[ i ];
			auto& pc = playerComponents[ i ];
			pc.cameraShake += cameraShake;

			wv::Vector3f flatVelocity = wv::Vector3f( rb.linearVelocity.x, 0.0f, rb.linearVelocity.z );
			pc.speedDiff = wv::Math::clamp( flatVelocity.length() / pc.moveSpeed, 0.0f, 1.0f );

			if ( m_isWalking && pc.speedDiff > 0.1f )
			{
				pc.walkTimer += deltaTime;

				pc.footstepTimer -= deltaTime;
				if ( pc.footstepTimer < 0 )
				{
					//footstepTimer = Random.Range( FootstepMinDelay, FootstepMaxDelay );
					if ( pc.isGrounded )
					{
						//FootstepAudioSource.pitch = Random.Range( 0.8f, 1.2f );
						//FootstepAudioSource.Play();
					}
				}
			}

			if ( pc.cameraShake > 0.0f )
				pc.cameraShake -= pc.cameraShakeDecay * deltaTime;
		}
	}

	
}

void PlayerMoveSystem::onPostUpdate()
{
	for ( wv::Archetype* archetype : getArchetypes() )
	{
		auto& entities = archetype->getEntities();
		auto& rigidbodies = archetype->getComponents<wv::RigidBodyComponent>();
		auto& playerComponents = archetype->getComponents<PlayerMoveComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			updateMouseLook      ( entities[ i ], rigidbodies[ i ], playerComponents[ i ] );
			updateCameraTransform( entities[ i ], rigidbodies[ i ], playerComponents[ i ] );
		}
	}
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
			updateMove( entities[ i ], rigidbodies[ i ], playerComponents[ i ] );
			capSpeed  ( entities[ i ], rigidbodies[ i ], playerComponents[ i ] );
		}
	}

	jump = false;
}

void PlayerMoveSystem::updateMove( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc )
{
	wv::Transform& transform = _entity->getTransform();
	
	wv::Vector3f forward = transform.forward();
	forward.y = 0.0f; 
	forward.normalize();
	
	wv::Vector3f right = transform.right();
	right.y = 0.0f; 
	right.normalize();

	wv::Vector3f moveDirection = 
		forward * m_moveInput.y + 
		right   * m_moveInput.x;

	if ( _pc.smoothAcceleration )
		_rb.addForce( moveDirection * _pc.acceleration * 10.0f, wv::ForceType_force );
	else
	{
		wv::Vector3f targetVelocity = moveDirection * _pc.moveSpeed;
		_rb.linearVelocity = wv::Vector3f{ targetVelocity.x, _rb.linearVelocity.y, targetVelocity.z };
	}

	wv::PhysicsSystem* physicsSystem = getWorld()->getSystem<wv::PhysicsSystem>();

	wv::RaycastHit hit;
	_pc.isGrounded = physicsSystem->sphereCast( 
		transform.position, 
		0.25f,
		{ 0.0f, -1.58, 0.0f },
		hit,
		{ wv::PhysicsLayer_NonMoving } 
	);

	if ( _pc.isGrounded )
		_rb.linearDamping = _pc.damping;
	else
		_rb.linearDamping = 0.0f;

}

void PlayerMoveSystem::capSpeed( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc )
{
	wv::Vector3f flatVelocity = wv::Vector3{ _rb.linearVelocity.x, 0.0f, _rb.linearVelocity.z };
	if ( flatVelocity.length() > _pc.moveSpeed )
	{
		wv::Vector3f limitedVelocity = flatVelocity.normalized() * _pc.moveSpeed;
		_rb.linearVelocity = wv::Vector3{ limitedVelocity.x, _rb.linearVelocity.y, limitedVelocity.z };
	}
}

void PlayerMoveSystem::updateMouseLook( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc )
{
	// 0.01f here so to keep CameraSensitivity in a 1-100 range
	wv::Vector2f cameraInput = m_lookInput * ( _pc.cameraSensitivity * 0.01f );
	
	_pc.yaw   -= cameraInput.x;
	_pc.pitch -= cameraInput.y;
	_pc.roll  = 0;
	
	_pc.pitch = wv::Math::clamp( _pc.pitch, -85.0f, 85.0f );
}

void PlayerMoveSystem::updateCameraTransform( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc )
{
	float currentRot = _pc.viewRotOffset + ( std::sinf( _pc.walkTimer * _pc.viewRotFrequency ) * _pc.viewRotting );
	float currentBob = std::sinf( _pc.walkTimer * _pc.viewBobbingSpeed ) * _pc.viewBobbing;
	if ( _pc.resetBobPosition )
		currentBob *= _pc.speedDiff;

	wv::Vector3f localPosition = wv::Vector3f( 0, ( _pc.cameraHeight - 1.0f ) + currentBob, 0 );

	float shakePitch = 0.0f;
	float shakeYaw = 0.0f;

	if ( _pc.cameraShake > 0 )
	{
		shakePitch = _pc.cameraShake * wv::Math::randomRange( -1.0f, 1.0f );
		shakeYaw   = _pc.cameraShake * wv::Math::randomRange( -1.0f, 1.0f );
	}

	if ( _pc.cameraEntity )
	{
		wv::Transform& cameraTransform = _pc.cameraEntity->getTransform();
		cameraTransform.position = localPosition;
		cameraTransform.rotation = wv::Rotorf::euler( 
			{ 
				_pc.pitch + shakePitch,
						    shakeYaw, 
				_pc.roll  + currentRot
			} 
		);
	}
	_entity->getTransform().rotation = wv::Rotorf::euler( 0.0f, _pc.yaw, 0.0f );
}


