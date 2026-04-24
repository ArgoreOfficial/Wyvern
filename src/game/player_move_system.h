#pragma once

#include <wv//serialize.h>
#include <wv/entity/ecs.h>
#include <wv/input/action_group.h>

namespace wv {

struct RigidBodyComponent;

}

struct PlayerMoveComponent
{
	// Camera
	wv::Entity* cameraEntity = nullptr;
	float cameraSensitivity  = 7.0f;
	float cameraHeight       = 1.6f;
	float cameraShakeDecay   = 0.5f;

	// Movement
	bool  smoothAcceleration = true;
	float acceleration       = 3;
	float moveSpeed          = 2;
	float damping            = 6;

	//[Header( "View Bobbing" )]
	float viewBobbing      = 0.05f;
	float viewBobbingSpeed = 7;
	bool  resetBobPosition = false;

	// In degrees

	float viewRotting      = 1.5f;
	float viewRotFrequency = 1.75f;
	float viewRotOffset    = 0.0f;
};


template<>
static void wv::serialize<PlayerMoveComponent>( SerializeInfo& _info )
{
	_info.name = "PlayerMoveComponent";
	_info.registerMember( &PlayerMoveComponent::cameraSensitivity, "cameraSensitivity" );
	_info.registerMember( &PlayerMoveComponent::cameraHeight, "cameraHeight" );
	_info.registerMember( &PlayerMoveComponent::cameraHeight, "cameraHeight" );

	_info.registerMember( &PlayerMoveComponent::smoothAcceleration, "smoothAcceleration" );
	_info.registerMember( &PlayerMoveComponent::acceleration, "acceleration" );
	_info.registerMember( &PlayerMoveComponent::moveSpeed, "moveSpeed" );
	_info.registerMember( &PlayerMoveComponent::damping, "damping" );

	_info.registerMember( &PlayerMoveComponent::viewBobbing, "viewBobbing" );
	_info.registerMember( &PlayerMoveComponent::viewBobbingSpeed, "viewBobbingSpeed" );
	_info.registerMember( &PlayerMoveComponent::resetBobPosition, "resetBobPosition" );

	_info.registerMember( &PlayerMoveComponent::viewRotting, "viewRotting" );
	_info.registerMember( &PlayerMoveComponent::viewRotFrequency, "viewRotFrequency" );
	_info.registerMember( &PlayerMoveComponent::viewRotOffset, "viewRotOffset" );
}


class PlayerMoveSystem : public wv::ISystem
{
public:
	virtual void configure( wv::ArchetypeConfig& _config ) override;

	virtual void onInitialize() override;

	virtual void onUpdate() override;
	virtual void onPostUpdate() override;

	virtual void onPhysicsUpdate() override;

protected:
	void updateMove( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc );
	void capSpeed( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc );

	void updateMouseLook( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc );
	void updateCameraTransform( wv::Entity* _entity, wv::RigidBodyComponent& _rb, PlayerMoveComponent& _pc );

	bool isLocked = false;



	bool m_isWalking = false;
	bool jump = false;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;



	float cameraShake = 0.0f;

	int groundLayerMask = 0;

	bool isGrounded = false;

	float speedDiff = 0.0f;
	float walkTimer = 0.0f;
	float footstepTimer = 0.0f;


	wv::Vector2f m_moveInput = {};
	wv::Vector2f m_lookInput = {};

	wv::ActionGroup* playerActionGroup = nullptr;
	wv::ActionID m_moveActionID = {};
	wv::ActionID m_shakeActionID = {};
	wv::ActionID m_mouseLockActionID = {};
};
