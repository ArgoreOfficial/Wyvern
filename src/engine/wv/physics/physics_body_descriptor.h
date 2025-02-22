#pragma once

#include <wv/math/vector3.h>
#include <wv/math/transform.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	enum PhysicsShape
	{
		WV_PHYSICS_NONE,
		WV_PHYSICS_SPHERE,
		WV_PHYSICS_BOX,
		WV_PHYSICS_CAPSULE,
		WV_PHYSICS_TAPERRED_CAPSULE,
		WV_PHYSICS_CYLINDER,
		WV_PHYSICS_CONVECT_HULL,
		WV_PHYSICS_PLANE,
		WV_PHYSICS_MESH,
		WV_PHYSICS_TERRAIN
	};

///////////////////////////////////////////////////////////////////////////////////////

	enum PhysicsKind
	{
		WV_PHYSICS_STATIC,
		WV_PHYSICS_DYANIMIC,
		WV_PHYSICS_KINEMATIC
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct IPhysicsBodyDesc
	{
		PhysicsShape shape = WV_PHYSICS_NONE;
		PhysicsKind kind = WV_PHYSICS_STATIC;
		Transformf transform{};
	};
	
///////////////////////////////////////////////////////////////////////////////////////

	struct PhysicsBoxDesc : public IPhysicsBodyDesc
	{
		PhysicsBoxDesc() { shape = WV_PHYSICS_BOX; }
		Vector3f halfExtent{};
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct PhysicsSphereDesc : public IPhysicsBodyDesc
	{
		PhysicsSphereDesc() { shape = WV_PHYSICS_SPHERE; }
		float radius = 1.0f;
	};

}