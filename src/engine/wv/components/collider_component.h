#pragma once

#include <wv/math/vector3.h>

namespace wv {

enum ColliderShape
{
	ColliderShape_box,
	ColliderShape_cylinder,
	ColliderShape_sphere
};

struct ColliderComponent
{
	ColliderShape shape = ColliderShape_box;

	// used with ColliderShape_box
	Vector3f boxSize{ 1.0f, 1.0f, 1.0f };

	// used with ColliderShape_cylinder
	float cylinderHeight = 1.0f;

	// used with ColliderShape_cylinder & ColliderShape_sphere
	float radius = 1.0f;
};

}