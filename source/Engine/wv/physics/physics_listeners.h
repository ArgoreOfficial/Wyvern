#pragma once

#ifdef WV_SUPPORT_JOLT_PHYSICS

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cJoltContactListener : public JPH::ContactListener
	{
	public:
		virtual JPH::ValidateResult OnContactValidate ( const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult ) override;
		virtual void                OnContactAdded    ( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings )     override;
		virtual void                OnContactPersisted( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings )     override;
		virtual void                OnContactRemoved  ( const JPH::SubShapeIDPair& inSubShapePair )                                                                                        override;

	};

///////////////////////////////////////////////////////////////////////////////////////

	class cJoltBodyActivationListener : public JPH::BodyActivationListener
	{
	public:
		virtual void OnBodyActivated  ( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData ) override;
		virtual void OnBodyDeactivated( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData ) override;

	};

}

#endif // WV_SUPPORT_JOLT_PHYSICS