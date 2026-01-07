#pragma once

#include <wv/entity/entity_system.h>
#include <wv/entity/entity_component.h>

#include <wv/input/action.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

namespace wv {

class PlayerControllerComponent;
class PlayerInputComponent;

class PlayerControllerSystem : public IEntitySystem
{
	WV_REFLECT_TYPE( PlayerControllerSystem )
public:
	PlayerControllerSystem() = default;
	~PlayerControllerSystem() { }

protected:
	virtual void initialize() override;
	virtual void shutdown() override;

	virtual void registerComponent( IEntityComponent* _component ) override;
	virtual void unregisterComponent( IEntityComponent* _component ) override;

	virtual void update( WorldUpdateContext& _ctx ) override;

	PlayerControllerComponent* m_player = nullptr;
	PlayerInputComponent*      m_playerInput = nullptr;

	wv::Vector2f m_move;
	wv::Vector3f m_velocity;

	uint32_t m_moveActionID = 0;
	uint32_t m_jumpActionID = 0;
};

}