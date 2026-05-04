#pragma once

#include <wv/serialize.h>
#include <wv/math/vector3.h>
#include <wv/rendering/mesh.h>

namespace wv {

enum ColliderShape
{
	ColliderShape_Box,
	ColliderShape_Cylinder,
	ColliderShape_Sphere,
	ColliderShape_Mesh
};

template <>
struct wv::EnumReflection<ColliderShape>
{
	static inline ReflectedEnums values = {
		{ "Box", ColliderShape_Box },
		{ "Cylinder", ColliderShape_Cylinder },
		{ "Sphere", ColliderShape_Sphere },
		{ "Mesh", ColliderShape_Mesh }
	};
};

struct ColliderComponent
{
	ColliderShape shape = ColliderShape_Box;

	// used with ColliderShape_Box
	Vector3f boxSize{ 1.0f, 1.0f, 1.0f };

	// used with ColliderShape_Cylinder
	float cylinderHeight = 1.0f;

	// used with ColliderShape_Cylinder & ColliderShape_Sphere
	float radius = 0.5f;

	Ref<MeshAsset> meshColliderAsset;

	static inline wv::Reflection reflection{
		wv::reflect( "shape", &ColliderComponent::shape ),
		wv::reflect( "boxSize", &ColliderComponent::boxSize ),
		wv::reflect( "cylinderHeight", &ColliderComponent::cylinderHeight ),
		wv::reflect( "radius", &ColliderComponent::radius ),
		wv::reflect( "meshColliderAsset", &ColliderComponent::meshColliderAsset )
	};
};

}