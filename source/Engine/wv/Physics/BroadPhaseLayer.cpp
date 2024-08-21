#include "BroadPhaseLayer.h"

#if defined( WV_SUPPORT_PHYSICS ) && defined( WV_SUPPORT_JOLT_PHYSICS )

wv::cBroadPhaseLayer::cBroadPhaseLayer()
{
	m_objectToBroadPhaseMapping[ Layers::STATIC ] = BroadPhaseLayers::STATIC;
	m_objectToBroadPhaseMapping[ Layers::DYNAMIC ] = BroadPhaseLayers::DYNAMIC;
}

JPH::uint wv::cBroadPhaseLayer::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}


JPH::BroadPhaseLayer wv::cBroadPhaseLayer::GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const
{
	JPH_ASSERT( inLayer < Layers::NUM_LAYERS );
	return m_objectToBroadPhaseMapping[ inLayer ];
}

#if defined( JPH_EXTERNAL_PROFILE ) || defined( JPH_PROFILE_ENABLED )
const char* wv::cBroadPhaseLayer::GetBroadPhaseLayerName( JPH::BroadPhaseLayer inLayer ) const
{
	switch( ( JPH::BroadPhaseLayer::Type )inLayer )
	{
	case ( JPH::BroadPhaseLayer::Type )BroadPhaseLayers::STATIC: return "NON_MOVING";
	case ( JPH::BroadPhaseLayer::Type )BroadPhaseLayers::DYNAMIC:     return "MOVING";
	default: JPH_ASSERT( false ); return "INVALID";
	}
}
#endif

#endif // WV_SUPPORT_PHYSICS && WV_SUPPORT_JOLT_PHYSICS