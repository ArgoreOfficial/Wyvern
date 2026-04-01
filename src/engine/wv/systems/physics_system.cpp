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
#include <wv/math/math.h>

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
	_config.addComponentType<RigidBodyComponent>();
}

void wv::PhysicsSystem::onInitialize()
{
	JPH::RegisterDefaultAllocator();

	JPH::Factory::sInstance = WV_NEW( JPH::Factory );

	JPH::RegisterTypes();

	m_tempAllocator = WV_NEW( JPH::TempAllocatorImpl, 10 * 1024 * 1024 );
	m_jobSystem = WV_NEW( JPH::JobSystemThreadPool, JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );

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
	

	// Static floor

	JPH::BoxShapeSettings floorShapeSettings( JPH::Vec3( 100.0f, 1.0f, 100.0f ) );
	floorShapeSettings.SetEmbedded();
	
	JPH::BodyCreationSettings floorSettings( 
		floorShapeSettings.Create().Get(),
		JPH::RVec3( 0.0f, -5.0f, 0.0f ), 
		JPH::Quat::sIdentity(), 
		JPH::EMotionType::Static, 
		Layers::NON_MOVING 
	);
	
	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	m_staticFloor = bodyInterface.CreateBody( floorSettings );
	bodyInterface.AddBody( m_staticFloor->GetID(), JPH::EActivation::DontActivate );
}

void wv::PhysicsSystem::onShutdown()
{
	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	
	bodyInterface.RemoveBody( m_staticFloor->GetID() );
	bodyInterface.DestroyBody( m_staticFloor->GetID() );

	for ( size_t i = 0; i < m_bodies.count(); i++ )
	{
		JPH::BodyID bodyID = m_bodies[ i + 1 ];
		bodyInterface.RemoveBody( bodyID );
		bodyInterface.DestroyBody( bodyID );
	}

	// Unregisters all types with the factory and cleans up the default material
	JPH::UnregisterTypes();

	WV_FREE( m_physicsSystem );

	WV_FREE( m_objectVsObjectLayerFilter );
	WV_FREE( m_objectVsBroadphaseLayerFilter );
	WV_FREE( m_broadPhaseLayerInterface );

	WV_FREE( m_jobSystem );
	WV_FREE( m_tempAllocator );

	WV_FREE( JPH::Factory::sInstance );
	JPH::Factory::sInstance = nullptr;
}

void wv::PhysicsSystem::onUpdate()
{
}

void wv::PhysicsSystem::onInternalPrePhysicsUpdate()
{
	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	uint32_t numCreatedBodies = 0;

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& rigidbodies = archetype->getComponents<RigidBodyComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			if ( rigidbodies[ i ].id == -1 )
			{
				Entity* ent = archetype->getEntity( i );
				wv::Vector3f pos = ent->getTransform().position;
				wv::Vector3f rot = ent->getTransform().rotation;

				JPH::BodyCreationSettings sphereSettings( 
					new JPH::SphereShape( 1.0f ), 
					JPH::RVec3( pos.x, pos.y, pos.z ), 
					JPH::Quat::sEulerAngles( 
						{ 
							rot.x, 
							rot.y, 
							rot.z 
						} 
					),
					JPH::EMotionType::Dynamic, 
					Layers::MOVING 
				);

				JPH::BodyID bodyID = bodyInterface.CreateAndAddBody( sphereSettings, JPH::EActivation::Activate );
				rigidbodies[ i ].id = m_bodies.emplace( bodyID );
				numCreatedBodies++;
			}
		}
	}

	if ( numCreatedBodies > 0 )
	{
		m_physicsSystem->OptimizeBroadPhase();
	}

}

void wv::PhysicsSystem::onInternalPhysicsUpdate( double _fixedDeltaTime )
{
	m_physicsSystem->Update( _fixedDeltaTime, 1, m_tempAllocator, m_jobSystem );

	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& rigidbodies = archetype->getComponents<RigidBodyComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			if ( rigidbodies[ i ].id == -1 )
				continue;

			JPH::BodyID bodyID = m_bodies.at( rigidbodies[ i ].id );
			auto pos = bodyInterface.GetPosition( bodyID );
			auto rot = bodyInterface.GetRotation( bodyID );
			auto rotEuler = rot.GetEulerAngles();

			Entity* entity = archetype->getEntity( i );
			
			entity->getTransform().position = { pos.GetX(), pos.GetY(), pos.GetZ() };
			entity->getTransform().rotation = { 
				wv::Math::degrees( rotEuler.GetX() ), 
				wv::Math::degrees( rotEuler.GetY() ), 
				wv::Math::degrees( rotEuler.GetZ() ) 
			};
		}
	}
}
