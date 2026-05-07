#pragma once

#include <wv/entity/entity.h>
#include <wv/reflection.h>

struct CrateComponent
{
	wv::Entity* cameraEntity = nullptr;
	wv::Entity* meshEntity = nullptr;
	wv::Vector3f positionOffset{};
	float jumpForceExponent = 1.0f;
	float jumpForceMultiplier = 10.0f;
	float jumpTorqueMultiplier = 1.0f;
	float visualChargeScale = 0.2f;

	wv::Vector3f currentAim{ 0.0f, 1.0f, 0.0f };
	float currentJumpCharge = 0.0f;
	float charge = 0.0f;

	wv::Vector3f jumpScale{ 1.0f, -1.0f, 1.0f };
	float chargeShake = 0.0f;

	static inline wv::Reflection reflection{
		wv::reflect( "cameraEntity", &CrateComponent::cameraEntity ),
		wv::reflect( "meshEntity", &CrateComponent::meshEntity ),
		wv::reflect( "positionOffset", &CrateComponent::positionOffset ),
		wv::reflect( "jumpForceExponent", &CrateComponent::jumpForceExponent ),
		wv::reflect( "jumpForceMultiplier", &CrateComponent::jumpForceMultiplier ),
		wv::reflect( "jumpTorqueMultiplier", &CrateComponent::jumpTorqueMultiplier ),
		wv::reflect( "visualChargeScale", &CrateComponent::visualChargeScale )
	};
};