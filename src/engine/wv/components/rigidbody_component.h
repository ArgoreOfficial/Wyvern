#pragma once

#include <wv/math/vector3.h>

namespace wv {

enum ForceType
{
	ForceType_force,       // force per second. Eg. pushing an object
	ForceType_impulse,     // instant force. Eg. jumping
	ForceType_acceleration // acceleration per second. Eg. falling
};

struct RigidBodyComponent
{
	size_t id = 0;

	float mass = 1.0f;

	Vector3<bool> lockPositionAxis{ false, false, false };
	Vector3<bool> lockRotationAxis{ false, false, false };

	Vector3f position{};
	Vector3f rotation{};
	Vector3f linearVelocity{};
	Vector3f angularVelocity{};

	void addForce( Vector3f _force, ForceType _forceType ) {
		constexpr double fixedDeltaTime = 0.01; // application.h:73

		switch ( _forceType )
		{
		case ForceType_force:        linearVelocity += ( _force * fixedDeltaTime ) / mass; break;
		case ForceType_impulse:      linearVelocity += _force / mass;                      break;
		case ForceType_acceleration: linearVelocity += _force * fixedDeltaTime;            break;
		}
	}
};

}