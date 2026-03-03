#pragma once

#include <wv/entity/entity_system.h>

class VtuberControllerSystem : public wv::IEntitySystem
{
	WV_REFLECT_TYPE( VtuberControllerSystem, wv::IEntitySystem )

public:
	VtuberControllerSystem() = default;
	~VtuberControllerSystem() { }

protected:
	virtual void initialize() override;
	virtual void shutdown() override;

	virtual void registerComponent( wv::IEntityComponent* _component ) override { };
	virtual void unregisterComponent( wv::IEntityComponent* _component ) override { };

	virtual void update( wv::WorldUpdateContext& _ctx ) override;

	virtual void onDebugRender() override;

	float m_time = 0.0f;
	float m_talkingTime = 0.0f;
	float m_interpolation = 0.0f;
	float m_currentMicValue = 0.0f;

	float m_yVelocity = 0.0f;

	float m_micAmplifier = 60.0f;
	float m_micThreshold = 0.5f;

	float m_interpolateInTime = 0.075f;
	float m_interpolateOutTime = 0.2f;

	float m_idleBobSpeed  = 1.0f;
	float m_idleBobAmount = 0.1f;

	float m_talkingBobSpeed  = 10.0f;
	float m_talkingBobAmount = 0.2f;

	float m_squishMultiplier = 6.0f;
};