#include "physics_system.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

JPH_SUPPRESS_WARNINGS

#include <wv/debug/log.h>
#include <wv/components/rigidbody_component.h>

/////

namespace wv {

namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide( JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2 ) const override
	{
		switch ( inObject1 )
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT( false );
			return false;
		}
	}
};

namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING( 0 );
static constexpr JPH::BroadPhaseLayer MOVING( 1 );
static constexpr uint32_t NUM_LAYERS( 2 );
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[ Layers::NON_MOVING ] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[ Layers::MOVING ]     = BroadPhaseLayers::MOVING;
	}

	virtual uint32_t GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override
	{
		JPH_ASSERT( inLayer < Layers::NUM_LAYERS );
		return mObjectToBroadPhase[ inLayer ];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName( JPH::BroadPhaseLayer inLayer ) const override
	{
		switch ( (JPH::BroadPhaseLayer::Type)inLayer )
		{
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default: JPH_ASSERT( false ); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[ Layers::NUM_LAYERS ];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide( JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2 ) const override
	{
		switch ( inLayer1 )
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT( false );
			return false;
		}
	}
};

}

void wv::PhysicsSystem::configure( ArchetypeConfig& _config )
{
}

void wv::PhysicsSystem::onInitialize()
{
	JPH::RegisterDefaultAllocator();

	JPH::Factory::sInstance = WV_NEW( JPH::Factory );

	JPH::RegisterTypes();

	JPH::TempAllocatorImpl tempAllocator( 10 * 1024 * 1024 );
	JPH::JobSystemThreadPool jobSystem( JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );

	m_broadPhaseLayerInterface = WV_NEW( BPLayerInterfaceImpl );
	m_objectVsBroadphaseLayerFilter = WV_NEW( ObjectVsBroadPhaseLayerFilterImpl );
	m_objectVsObjectLayerFilter = WV_NEW( ObjectLayerPairFilterImpl );

	m_physicsSystem = WV_NEW( JPH::PhysicsSystem );

	m_physicsSystem->Init(
		m_maxPhysicsBodies, m_numBodyMutexes, m_maxBodyPairs, m_maxContactConstraints,
		*(JPH::BroadPhaseLayerInterface*)m_broadPhaseLayerInterface,
		*(JPH::ObjectVsBroadPhaseLayerFilter*)m_objectVsBroadphaseLayerFilter,
		*(JPH::ObjectLayerPairFilter*)m_objectVsObjectLayerFilter
	);


}

void wv::PhysicsSystem::onShutdown()
{
	// Unregisters all types with the factory and cleans up the default material
	JPH::UnregisterTypes();

	WV_FREE( m_physicsSystem );

	WV_FREE( m_objectVsObjectLayerFilter );
	WV_FREE( m_objectVsBroadphaseLayerFilter );
	WV_FREE( m_broadPhaseLayerInterface );

	WV_FREE( JPH::Factory::sInstance );
	JPH::Factory::sInstance = nullptr;
}

void wv::PhysicsSystem::onUpdate()
{
}
