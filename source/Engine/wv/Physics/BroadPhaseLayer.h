#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace wv
{
	namespace Layers
	{
		static constexpr JPH::ObjectLayer STATIC = 0;
		static constexpr JPH::ObjectLayer DYNAMIC = 1;
		static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
	};

	namespace BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer STATIC( 0 );
		static constexpr JPH::BroadPhaseLayer DYNAMIC( 1 );
		static constexpr JPH::uint NUM_LAYERS( 2 );
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cBroadPhaseLayer : public JPH::BroadPhaseLayerInterface
	{
	
	public:

		cBroadPhaseLayer();

		virtual JPH::uint GetNumBroadPhaseLayers() const override;
		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override;
	#if defined( JPH_EXTERNAL_PROFILE ) || defined( JPH_PROFILE_ENABLED )
		virtual const char* GetBroadPhaseLayerName( JPH::BroadPhaseLayer inLayer ) const override;
	#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

///////////////////////////////////////////////////////////////////////////////////////

	private:
	
		JPH::BroadPhaseLayer m_objectToBroadPhaseMapping[ Layers::NUM_LAYERS ];

	};

///////////////////////////////////////////////////////////////////////////////////////

	class cObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		virtual bool ShouldCollide( JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2 ) const override
		{
			switch( inLayer1 )
			{
			case Layers::STATIC:  return inLayer2 == wv::BroadPhaseLayers::DYNAMIC;
			case Layers::DYNAMIC: return true;

			default: return false;
			}
		}
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
	{
	public:
		virtual bool ShouldCollide( JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2 ) const override
		{
			switch( inObject1 )
			{
			case Layers::STATIC:  return inObject2 == Layers::DYNAMIC; // Static bodies only collides with dynamic bodies
			case Layers::DYNAMIC: return true;                         // Dynamic bodies collides with everything

			default: return false;
			}
		}
	};

}


