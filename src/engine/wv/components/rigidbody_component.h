#pragma once

#include <wv/serialize.h>
#include <wv/math/vector3.h>

namespace wv {

enum ForceType
{
	ForceType_force,       // force per second. Eg. pushing an object
	ForceType_impulse,     // instant force. Eg. jumping
	ForceType_acceleration // acceleration per second. Eg. falling
};

enum BodyType
{
	BodyType_Static,
	BodyType_Dynamic,
	BodyType_Kinematic
};

struct RigidBodyComponentInternal
{
private:
	friend struct RigidBodyComponent;
	friend class PhysicsSystem;

	Vector3f previousPosition{};
	Rotorf   previousRotation{};

	double fixedDeltaTime = 0.02;
};

struct RigidBodyComponent
{
	size_t id = 0;

	BodyType bodyType = BodyType_Dynamic;

	float mass = 1.0f;
	float linearDamping = 0.05f;

	Vector3<bool> lockPositionAxis{ false, false, false };
	Vector3<bool> lockRotationAxis{ false, false, false };

	Vector3f position{};
	Rotorf   rotation{};
	Vector3f linearVelocity{};
	Vector3f angularVelocity{};

	void addForce( Vector3f _force, ForceType _forceType ) {
		switch ( _forceType )
		{
		case ForceType_force:        linearVelocity += ( _force * internal.fixedDeltaTime ) / mass; break;
		case ForceType_impulse:      linearVelocity += _force / mass;                               break;
		case ForceType_acceleration: linearVelocity += _force * internal.fixedDeltaTime;            break;
		}
	}

	RigidBodyComponentInternal internal;

	static inline wv::Reflection reflection{
		wv::reflect( "bodyType", &RigidBodyComponent::bodyType ),
		wv::reflect( "mass", &RigidBodyComponent::mass ),
		wv::reflect( "linearDamping", &RigidBodyComponent::linearDamping ),
		wv::reflect( "lockPositionAxis", &RigidBodyComponent::lockPositionAxis ),
		wv::reflect( "lockRotationAxis", &RigidBodyComponent::lockRotationAxis )
	};
};

}