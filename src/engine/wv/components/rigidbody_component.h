#pragma once

#include <wv/math/vector3.h>

namespace wv {

struct RigidBodyComponent
{
	size_t id = 0;

	Vector3f velocity;
};

}