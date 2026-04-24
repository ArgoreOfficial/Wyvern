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
#include <wv/entity/world.h>

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
	m_debugName = "PhysicsSystem";

	setUpdateMode( UpdateMode_always );

	_config.addComponentType<RigidBodyComponent>();
	_config.addComponentType<ColliderComponent>();
}

void wv::PhysicsSystem::onComponentAdded( Archetype* _archetype, size_t _index )
{
	Entity* ent = _archetype->getEntities()[ _index ];
	ColliderComponent&  collider  = _archetype->getComponents<ColliderComponent>()[ _index ];
	RigidBodyComponent& rigidbody = _archetype->getComponents<RigidBodyComponent>()[ _index ];
	Transform& tfm = ent->getTransform();

	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	rigidbody.position = ent->getTransform().position;
	rigidbody.rotation = ent->getTransform().rotation;
	rigidbody.internal.previousPosition = rigidbody.position;
	rigidbody.internal.previousRotation = rigidbody.rotation;
	rigidbody.internal.fixedDeltaTime = getApp()->getPhysicsDeltaTime();

	JPH::BodyCreationSettings bodySetting{};
	Vector4f quat = rigidbody.rotation.toQuaternion();

	JPH::RVec3 jphPos = JPH::RVec3( rigidbody.position.x, rigidbody.position.y, rigidbody.position.z );
	JPH::Quat  jphRot = JPH::Quat( quat.x, quat.y, quat.z, quat.w );

	JPH::EMotionType motionType = JPH::EMotionType::Dynamic;
	switch ( rigidbody.bodyType )
	{
	case BodyType_Static:    motionType = JPH::EMotionType::Static; break;
	case BodyType_Dynamic:   motionType = JPH::EMotionType::Dynamic; break;
	case BodyType_Kinematic: motionType = JPH::EMotionType::Kinematic; break;
	}
	
	auto layers = Layers::MOVING;

	switch ( collider.shape )
	{
	case ColliderShape_box:
	{
		bodySetting = JPH::BodyCreationSettings(
			new JPH::BoxShape( 
				{ 
					collider.boxSize.x / 2.0f * tfm.scale.x, 
					collider.boxSize.y / 2.0f * tfm.scale.y, 
					collider.boxSize.z / 2.0f * tfm.scale.z 
				} ),
			jphPos, jphRot, motionType, layers
		);
	} break;

	case ColliderShape_cylinder:
	{
		bodySetting = JPH::BodyCreationSettings(
			new JPH::CylinderShape( collider.cylinderHeight / 2.0f, collider.radius ),
			jphPos, jphRot, motionType, layers
		);
	} break;

	case ColliderShape_sphere:
	{
		bodySetting = JPH::BodyCreationSettings(
			new JPH::SphereShape( wv::Math::max( 0.0000001f, collider.radius ) ),
			jphPos, jphRot, motionType, layers
		);
	} break;
	}

	bodySetting.mMassPropertiesOverride = {};
	bodySetting.mMassPropertiesOverride.ScaleToMass( rigidbody.mass ); // actual mass in kg
	bodySetting.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;

	bodySetting.mAllowedDOFs = JPH::EAllowedDOFs::None;
	if ( !rigidbody.lockPositionAxis.x ) bodySetting.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationX;
	if ( !rigidbody.lockPositionAxis.y ) bodySetting.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationY;
	if ( !rigidbody.lockPositionAxis.z ) bodySetting.mAllowedDOFs |= JPH::EAllowedDOFs::TranslationZ;
	if ( !rigidbody.lockRotationAxis.x ) bodySetting.mAllowedDOFs |= JPH::EAllowedDOFs::RotationX;
	if ( !rigidbody.lockRotationAxis.y ) bodySetting.mAllowedDOFs |= JPH::EAllowedDOFs::RotationY;
	if ( !rigidbody.lockRotationAxis.z ) bodySetting.mAllowedDOFs |= JPH::EAllowedDOFs::RotationZ;

	bodySetting.mLinearDamping = rigidbody.linearDamping;

	JPH::BodyID bodyID = bodyInterface.CreateAndAddBody( bodySetting, JPH::EActivation::Activate );
	rigidbody.id = m_bodies.push( bodyID );
	m_numPhysicsBodyChanges++;
}

void wv::PhysicsSystem::onComponentRemoved( Archetype* _archetype, size_t _index )
{
	int id = _archetype->getComponents<RigidBodyComponent>()[ _index ].id;
	
	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	JPH::BodyID bodyID = m_bodies[ id ];
	bodyInterface.RemoveBody ( bodyID );
	bodyInterface.DestroyBody( bodyID );
	m_bodies.erase( id );

	m_numPhysicsBodyChanges++;
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
}

void wv::PhysicsSystem::onShutdown()
{
	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	
	for ( JPH::BodyID bodyID : m_bodies )
	{
		bodyInterface.RemoveBody( bodyID );
		bodyInterface.DestroyBody( bodyID );
	}
	m_bodies.clear();

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

void wv::PhysicsSystem::onUpdate()
{
	if ( getWorld()->isEditorState() )
	{
		JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	// set velocities
		for ( Archetype* archetype : getArchetypes() )
		{
			auto& rigidbodies = archetype->getComponents<RigidBodyComponent>();
			auto& entities = archetype->getEntities();

			for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
			{
				RigidBodyComponent& rigidbody = rigidbodies[ i ];
				if ( !m_bodies.contains( rigidbody.id ) )
					continue;

				Entity* entity = entities[ i ];
				Transform& tfm = entity->getTransform();
				JPH::BodyID bodyID = m_bodies.at( rigidbody.id );
				
				auto jquat = bodyInterface.GetRotation( bodyID );
				Vector4f quat = rigidbody.rotation.toQuaternion();

				rigidbody.position = tfm.position;
				rigidbody.rotation = tfm.rotation;
				
				// Set pos, rot, vel
				bodyInterface.SetPositionRotationAndVelocity(
					bodyID,
					{
						rigidbody.position.x,
						rigidbody.position.y,
						rigidbody.position.z
					},
				{
					quat.x,
					quat.y,
					quat.z,
					quat.w
				},
				{
					rigidbody.linearVelocity.x,
					rigidbody.linearVelocity.y,
					rigidbody.linearVelocity.z
				},
				{
					rigidbody.angularVelocity.x,
					rigidbody.angularVelocity.y,
					rigidbody.angularVelocity.z
				}
				);
			}
		}
	}
}

void wv::PhysicsSystem::onDebugRender()
{
	if ( m_drawEnabled )
	{
		JPH::BodyManager::DrawSettings drawSettings{};
		drawSettings.mDrawShape = m_drawEnabled;
		drawSettings.mDrawShapeWireframe = m_drawWireframe;
		drawSettings.mDrawBoundingBox = m_drawBoundingBox;
		drawSettings.mDrawCenterOfMassTransform = m_drawCenterOfMassTransform;
		drawSettings.mDrawWorldTransform = m_drawWorldTransform;
		drawSettings.mDrawVelocity = m_drawVelocity;
		drawSettings.mDrawMassAndInertia = m_drawMassAndInertia;
		m_physicsSystem->DrawBodies( drawSettings, JPH::DebugRenderer::sInstance );
	}
}

void wv::PhysicsSystem::onEditorRender()
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
}

void wv::PhysicsSystem::onInternalPrePhysicsUpdate()
{
	if ( m_numPhysicsBodyChanges > 0 )
		m_physicsSystem->OptimizeBroadPhase();

	m_numPhysicsBodyChanges = 0;
}

void wv::PhysicsSystem::onInternalPhysicsUpdate( double _fixedDeltaTime )
{
	JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	// set velocities
	for ( Archetype* archetype : getArchetypes() )
	{
		for ( RigidBodyComponent& rigidbody : archetype->getComponents<RigidBodyComponent>() )
		{
			if ( !m_bodies.contains( rigidbody.id ) )
				continue;

			JPH::BodyID bodyID = m_bodies.at( rigidbody.id );
			rigidbody.internal.fixedDeltaTime = _fixedDeltaTime;

			if ( rigidbody.bodyType != BodyType_Static )
			{
				JPH::BodyLockWrite lock( m_physicsSystem->GetBodyLockInterface(), bodyID );
				if ( !lock.Succeeded() )
					continue;

				JPH::Body& body = lock.GetBody();
				JPH::MotionProperties* mp = body.GetMotionProperties();

				// Set damping 
				mp->SetLinearDamping( rigidbody.linearDamping );
				//mp->SetAngularDamping( rigidbody.angularDamping );
			}

			auto jquat = bodyInterface.GetRotation( bodyID );
			Vector4f quat = rigidbody.rotation.toQuaternion();

			// Set pos, rot, vel
			bodyInterface.SetPositionRotationAndVelocity(
				bodyID,
				{
					rigidbody.position.x,
					rigidbody.position.y,
					rigidbody.position.z
				},
				{
					quat.x,
					quat.y,
					quat.z,
					quat.w
				},
				{
					rigidbody.linearVelocity.x,
					rigidbody.linearVelocity.y,
					rigidbody.linearVelocity.z
				},
				{
					rigidbody.angularVelocity.x,
					rigidbody.angularVelocity.y,
					rigidbody.angularVelocity.z
				}
			);
		}
	}

	// do jolt physics update
	m_physicsSystem->Update( _fixedDeltaTime, 1, m_tempAllocator, m_jobSystem );

	// update rigidbody data
	for ( Archetype* archetype : getArchetypes() )
	{
		auto& entities = archetype->getEntities();
		auto& rigidbodies = archetype->getComponents<RigidBodyComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			RigidBodyComponent& rigidbody = rigidbodies[ i ];

			if ( rigidbody.id == -1 )
				continue;

			JPH::BodyID bodyID = m_bodies.at( rigidbody.id );
			JPH::Quat rotation = bodyInterface.GetRotation( bodyID );
			JPH::Vec3 position = bodyInterface.GetPosition( bodyID );
			JPH::Vec3 linearVelocity  = bodyInterface.GetLinearVelocity( bodyID );
			JPH::Vec3 angularVelocity = bodyInterface.GetAngularVelocity( bodyID );
			
			rigidbody.internal.previousPosition = rigidbody.position;
			rigidbody.position = { 
				position.GetX(),
				position.GetY(), 
				position.GetZ() 
			};

			rigidbody.internal.previousRotation = rigidbody.rotation;
			
			const Vector4f quat{
				rotation.GetX(),
				rotation.GetY(),
				rotation.GetZ(),
				rotation.GetW()
			};
			rigidbody.rotation = Rotorf::fromQuaternion( quat );

			rigidbody.linearVelocity  = { 
				linearVelocity.GetX(), 
				linearVelocity.GetY(), 
				linearVelocity.GetZ() 
			};

			rigidbody.angularVelocity = { 
				angularVelocity.GetX(),
				angularVelocity.GetY(), 
				angularVelocity.GetZ() 
			};
		}
	}
}

void wv::PhysicsSystem::onInternalPostPhysicsUpdate( double _fraction )
{
	for ( Archetype* archetype : getArchetypes() )
	{
		auto& entities = archetype->getEntities();
		auto& rigidbodies = archetype->getComponents<RigidBodyComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			RigidBodyComponent& rigidbody = rigidbodies[ i ];

			if ( rigidbody.id == -1 )
				continue;

			Entity* entity = entities[ i ];
			entity->getTransform().position = wv::Math::lerp( rigidbody.internal.previousPosition, rigidbody.position, _fraction );
			entity->getTransform().rotation = wv::Math::lerp( rigidbody.internal.previousRotation, rigidbody.rotation, _fraction );
		}
	}
}
