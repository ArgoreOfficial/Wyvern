#include "broad_phase_layer.h"

///////////////////////////////////////////////////////////////////////////////////////

#if defined( WV_SUPPORT_PHYSICS ) && defined( WV_SUPPORT_JOLT_PHYSICS )

wv::BroadPhaseLayer::BroadPhaseLayer()
{
	m_objectToBroadPhaseMapping[ Layers::STATIC ] = BroadPhaseLayers::STATIC;
	m_objectToBroadPhaseMapping[ Layers::DYNAMIC ] = BroadPhaseLayers::DYNAMIC;
}

///////////////////////////////////////////////////////////////////////////////////////

JPH::uint wv::BroadPhaseLayer::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}

///////////////////////////////////////////////////////////////////////////////////////

JPH::BroadPhaseLayer wv::BroadPhaseLayer::GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const
{
	JPH_ASSERT( inLayer < Layers::NUM_LAYERS );
	return m_objectToBroadPhaseMapping[ inLayer ];
}

#if defined( JPH_EXTERNAL_PROFILE ) || defined( JPH_PROFILE_ENABLED )
const char* wv::BroadPhaseLayer::GetBroadPhaseLayerName( JPH::BroadPhaseLayer inLayer ) const
{
	switch( ( JPH::BroadPhaseLayer::Type )inLayer )
	{
	case ( JPH::BroadPhaseLayer::Type )BroadPhaseLayers::STATIC: return "NON_MOVING";
	case ( JPH::BroadPhaseLayer::Type )BroadPhaseLayers::DYNAMIC:     return "MOVING";
	default: JPH_ASSERT( false ); return "INVALID";
	}
}
#endif

bool wv::ObjectLayerPairFilter::ShouldCollide( JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2 ) const
{
	switch( inObject1 )
	{
	case Layers::STATIC:  return inObject2 == Layers::DYNAMIC; // Static bodies only collides with dynamic bodies
	case Layers::DYNAMIC: return true;                         // Dynamic bodies collides with everything

	default: return false;
	}
}

bool wv::ObjectVsBroadPhaseLayerFilter::ShouldCollide( JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2 ) const
{
	switch( inLayer1 )
	{
	case Layers::STATIC:  return inLayer2 == wv::BroadPhaseLayers::DYNAMIC;
	case Layers::DYNAMIC: return true;

	default: return false;
	}
}

#endif // WV_SUPPORT_PHYSICS && WV_SUPPORT_JOLT_PHYSICS