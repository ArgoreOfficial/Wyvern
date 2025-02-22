#ifdef WV_SUPPORT_JOLT_PHYSICS

#include "physics_listener.h"

#include <wv/debug/log.h>

///////////////////////////////////////////////////////////////////////////////////////

JPH::ValidateResult wv::cJoltContactListener::OnContactValidate( const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult )
{
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Physics contact validate callback\n\n" );
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltContactListener::OnContactAdded( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings )
{
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Physics contact was added\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltContactListener::OnContactPersisted( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings )
{
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Physics contact was persisted\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltContactListener::OnContactRemoved( const JPH::SubShapeIDPair& inSubShapePair )
{
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Physics contact was removed\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltBodyActivationListener::OnBodyActivated( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData )
{
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Physics body was activated\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltBodyActivationListener::OnBodyDeactivated( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData )
{
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Physics body went to sleep\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

#endif // WV_SUPPORT_JOLT_PHYSICS
