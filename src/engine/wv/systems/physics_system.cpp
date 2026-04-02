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
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

JPH_SUPPRESS_WARNINGS

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>
#include <wv/math/math.h>
#include <wv/rendering/renderer.h>

#ifdef WV_SUPPORT_IMGUI
#include <imgui/imgui.h>
#endif

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

class PhysicsDebugRenderer : public JPH::DebugRendererSimple
{
public:
	PhysicsDebugRenderer() {
		m_wvRenderer = wv::getApp()->getRenderer();
	}

	virtual void DrawLine( JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor ) override
	{
		m_wvRenderer->addDebugLine(
			{ inFrom.GetX(), inFrom.GetY(), inFrom.GetZ() },
			{ inTo.GetX(), inTo.GetY(), inTo.GetZ() }
		);
	}

	virtual void DrawTriangle( JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow ) override
	{
		// Implement
	}

	virtual void DrawText3D( JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight ) override
	{
		// Implement
	}

	wv::Renderer* m_wvRenderer = nullptr;
};

}

void wv::PhysicsSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<RigidBodyComponent>();
	_config.addComponentType<ColliderComponent>();
}

void wv::PhysicsSystem::onComponentAdded( Archetype* _archetype, size_t _index )
{
	Entity* ent = _archetype->getEntities()[ _index ];
	ColliderComponent&  collider  = _archetype->getComponents<ColliderComponent>()[ _index ];
	RigidBodyComponent& rigidbody = _archetype->getComponents<RigidBodyComponent>()[ _index ];

	if ( rigidbody.id != -1 )
		return;

	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	wv::Vector3f pos = ent->getTransform().position;
	wv::Vector3f rot = ent->getTransform().rotation;
	JPH::BodyCreationSettings shapeSetting{};

	auto jphPos = JPH::RVec3( pos.x, pos.y, pos.z );
	auto jphRot = JPH::Quat::sEulerAngles( { rot.x, rot.y, rot.z } );
	auto motionType = JPH::EMotionType::Dynamic;
	auto layers = Layers::MOVING;

	switch ( collider.shape )
	{
	case ColliderShape_box:
	{
		shapeSetting = JPH::BodyCreationSettings(
			new JPH::BoxShape( { collider.boxSize.x / 2.0f, collider.boxSize.y / 2.0f, collider.boxSize.z / 2.0f } ),
			jphPos, jphRot, motionType, layers
		);
	} break;

	case ColliderShape_cylinder:
	{
		shapeSetting = JPH::BodyCreationSettings(
			new JPH::CylinderShape( collider.cylinderHeight / 2.0f, collider.radius ),
			jphPos, jphRot, motionType, layers
		);
	} break;

	case ColliderShape_sphere:
	{
		shapeSetting = JPH::BodyCreationSettings(
			new JPH::SphereShape( wv::Math::max( 0.0000001f, collider.radius ) ),
			jphPos, jphRot, motionType, layers
		);
	} break;
	}

	JPH::BodyID bodyID = bodyInterface.CreateAndAddBody( shapeSetting, JPH::EActivation::Activate );
	rigidbody.id = m_bodies.emplace( bodyID );
	m_numPhysicsBodyChanges++;
}

void wv::PhysicsSystem::onComponentRemoved( Archetype* _archetype, size_t _index )
{
	Debug::Print( "Physics Component Removed\n" );
}

void wv::PhysicsSystem::onInitialize()
{
	JPH::RegisterDefaultAllocator();

	JPH::Factory::sInstance = WV_NEW( JPH::Factory );

	JPH::RegisterTypes();
	
	m_tempAllocator = WV_NEW( JPH::TempAllocatorImpl, 10 * 1024 * 1024 );
	m_jobSystem = WV_NEW( JPH::JobSystemThreadPool, JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );
	m_debugRenderer = WV_NEW( wv::PhysicsDebugRenderer );

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

	WV_FREE( m_debugRenderer );
	WV_FREE( m_jobSystem );
	WV_FREE( m_tempAllocator );

	WV_FREE( JPH::Factory::sInstance );
	JPH::Factory::sInstance = nullptr;
}

void wv::PhysicsSystem::onDebugRender()
{
	if( ImGui::Begin( "Physics Debug" ) )
	{
		ImGui::Checkbox( "Enable Debug Draw",             &m_drawEnabled );
		ImGui::BeginDisabled( !m_drawEnabled );
		ImGui::Checkbox( "Draw Wireframe",                &m_drawWireframe );
		ImGui::Checkbox( "Draw Bounding Box",             &m_drawBoundingBox );
		ImGui::Checkbox( "Draw Center of Mass Transform", &m_drawCenterOfMassTransform );
		ImGui::Checkbox( "Draw World Transform",          &m_drawWorldTransform );
		ImGui::Checkbox( "Draw Velocity",                 &m_drawVelocity );
		ImGui::Checkbox( "Draw Mass and Inertia",         &m_drawMassAndInertia );
		ImGui::EndDisabled();
	}
	ImGui::End();

	if ( m_drawEnabled )
	{
		JPH::BodyManager::DrawSettings drawSettings{};
		drawSettings.mDrawShape = m_drawEnabled;
		drawSettings.mDrawShapeWireframe        = m_drawWireframe;
		drawSettings.mDrawBoundingBox           = m_drawBoundingBox;
		drawSettings.mDrawCenterOfMassTransform = m_drawCenterOfMassTransform;
		drawSettings.mDrawWorldTransform        = m_drawWorldTransform;
		drawSettings.mDrawVelocity              = m_drawVelocity;
		drawSettings.mDrawMassAndInertia        = m_drawMassAndInertia;
		m_physicsSystem->DrawBodies( drawSettings, JPH::DebugRenderer::sInstance );
	}
}

void wv::PhysicsSystem::onInternalPrePhysicsUpdate()
{
	if ( m_numPhysicsBodyChanges > 0 )
		m_physicsSystem->OptimizeBroadPhase();

	m_numPhysicsBodyChanges = 0;
}

void wv::PhysicsSystem::onInternalPhysicsUpdate( double _fixedDeltaTime )
{
	m_physicsSystem->Update( _fixedDeltaTime, 1, m_tempAllocator, m_jobSystem );

	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& rigidbodies = archetype->getComponents<RigidBodyComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			if ( rigidbodies[ i ].id == -1 )
				continue;

			JPH::BodyID bodyID = m_bodies.at( rigidbodies[ i ].id );
			auto pos = bodyInterface.GetPosition( bodyID );
			auto rot = bodyInterface.GetRotation( bodyID );
			auto rotEuler = rot.GetEulerAngles();

			Entity* entity = entities[ i ];
			entity->getTransform().position = { pos.GetX(), pos.GetY(), pos.GetZ() };
			entity->getTransform().rotation = { 
				wv::Math::degrees( rotEuler.GetX() ), 
				wv::Math::degrees( rotEuler.GetY() ), 
				wv::Math::degrees( rotEuler.GetZ() ) 
			};
		}
	}
}
