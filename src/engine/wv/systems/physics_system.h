#pragma once

#include <wv/entity/ecs.h>
#include <wv/slot_map.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace JPH {
class PhysicsSystem;
class TempAllocatorImpl;
class JobSystemThreadPool;
class Body;
}

namespace wv {

class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class ObjectLayerPairFilterImpl;
class PhysicsDebugRenderer;

class PhysicsObjectContainer;

enum PhysicsLayer
{
	PhysicsLayer_NonMoving = 0,
	PhysicsLayer_Moving = 1
};

struct RaycastHit
{

};

class PhysicsSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override;

	virtual void onComponentAdded( Archetype* _archetype, size_t _index ) override;
	virtual void onComponentRemoved( Archetype* _archetype, size_t _index ) override;

	virtual void onInitialize() override;
	virtual void onShutdown() override;

	virtual void onUpdate() override;

	virtual void onDebugRender() override;
	virtual void onEditorRender() override;

	void onInternalPrePhysicsUpdate(); // runs once
	void onInternalPhysicsUpdate( double _fixedDeltaTime ); // may run multiple times
	void onInternalPostPhysicsUpdate( double _fraction );

	bool rayCast( Vector3f _origin, Vector3f _direction, RaycastHit& _outHit, const std::vector<PhysicsLayer>& _filterLayers = {} );
	bool sphereCast( Vector3f _origin, float _radius, Vector3f _direction, RaycastHit& _outHit, const std::vector<PhysicsLayer>& _filterLayers = {} );

protected:
	wv::SlotMap<JPH::BodyID> m_bodies;

	BPLayerInterfaceImpl*              m_broadPhaseLayerInterface      = nullptr;
	ObjectVsBroadPhaseLayerFilterImpl* m_objectVsBroadphaseLayerFilter = nullptr;
	ObjectLayerPairFilterImpl*         m_objectVsObjectLayerFilter     = nullptr;
	PhysicsDebugRenderer*              m_debugRenderer = nullptr;

	JPH::TempAllocatorImpl*   m_tempAllocator = nullptr;
	JPH::JobSystemThreadPool* m_jobSystem     = nullptr;
	JPH::PhysicsSystem*       m_physicsSystem = nullptr;

	const uint32_t m_maxPhysicsBodies = 65536;
	const uint32_t m_numBodyMutexes = 0;
	const uint32_t m_maxBodyPairs = 65536;
	const uint32_t m_maxContactConstraints = 10240;

	// number of removes and adds
	size_t m_numPhysicsBodyChanges = 0;

	// Debug Drawing

	bool m_drawEnabled               = false;
	bool m_drawWireframe             = false;
	bool m_drawBoundingBox           = false;
	bool m_drawCenterOfMassTransform = false;
	bool m_drawWorldTransform        = false;
	bool m_drawVelocity              = false;
	bool m_drawMassAndInertia        = false;
};

}