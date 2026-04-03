#pragma once

#include <wv/math/vector3.h>

namespace wv {

struct RigidBodyComponent
{
	size_t id = 0;

	Vector3f position;
	Vector3f rotation;
	Vector3f linearVelocity;
	Vector3f angularVelocity;
};

}