#pragma once

#include <wv/entity/ecs.h>

#include <wv/input/action.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

namespace wv {

struct PlayerControllerComponent;
struct PlayerInputComponent;

class PlayerControllerSystem : public ISystem
{
//	WV_REFLECT_TYPE( PlayerControllerSystem, IEntitySystem )
public:
	
	virtual void configure( ArchetypeConfig& _config ) override;
	virtual void onInitialize() override;
	
	virtual void onUpdate() override;

	uint32_t m_moveActionID = 0;
	uint32_t m_jumpActionID = 0;
};

}