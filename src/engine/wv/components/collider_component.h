#pragma once

#include <wv/serialize.h>
#include <wv/math/vector3.h>
#include <wv/rendering/mesh.h>

namespace wv {

enum ColliderShape
{
	ColliderShape_box,
	ColliderShape_cylinder,
	ColliderShape_sphere,
	ColliderShape_mesh
};

struct ColliderComponent
{
	ColliderShape shape = ColliderShape_box;

	// used with ColliderShape_box
	Vector3f boxSize{ 1.0f, 1.0f, 1.0f };

	// used with ColliderShape_cylinder
	float cylinderHeight = 1.0f;

	// used with ColliderShape_cylinder & ColliderShape_sphere
	float radius = 0.5f;

	Ref<MeshAsset> meshColliderAsset;
};

template<>
static void serialize<ColliderComponent>( SerializeInfo& _info )
{
	_info.name = "ColliderComponent";
	_info.registerMember( &ColliderComponent::shape, "shape" );
	_info.registerMember( &ColliderComponent::boxSize, "boxSize" );
	_info.registerMember( &ColliderComponent::cylinderHeight, "cylinderHeight" );
	_info.registerMember( &ColliderComponent::radius, "radius" );
	_info.registerMember( &ColliderComponent::meshColliderAsset, "meshColliderAsset" );
}

}