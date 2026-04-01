#pragma once

#include <wv/entity/ecs.h>

namespace JPH {
class PhysicsSystem;
}

namespace wv {

class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class ObjectLayerPairFilterImpl;

class PhysicsSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override;

	virtual void onInitialize() override;
	virtual void onShutdown() override;

	virtual void onUpdate() override;

protected:
	BPLayerInterfaceImpl* m_broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl* m_objectVsBroadphaseLayerFilter;
	ObjectLayerPairFilterImpl* m_objectVsObjectLayerFilter;

	JPH::PhysicsSystem* m_physicsSystem;

	const uint32_t m_maxPhysicsBodies = 1024;
	const uint32_t m_numBodyMutexes = 0;
	const uint32_t m_maxBodyPairs = 1024;
	const uint32_t m_maxContactConstraints = 1024;
};

}