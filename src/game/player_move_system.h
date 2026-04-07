#pragma once

#include <wv/entity/ecs.h>
#include <wv/input/action.h>

namespace wv {

struct RigidBodyComponent;

}

struct PlayerMoveComponent
{
	wv::Entity* cameraEntity;

	bool  smoothAcceleration = false;
	float acceleration = 3;
	float moveSpeed = 2;
	float damping = 6;
};

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

	bool m_isWalking = false;

	bool jump = false;
	wv::Vector2f m_moveInput = {};
	wv::ActionID m_moveActionID = {};
	wv::ActionID m_jumpActionID = {};
};
