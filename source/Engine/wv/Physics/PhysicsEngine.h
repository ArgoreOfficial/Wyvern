#pragma once

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_JOLT_PHYSICS
namespace JPH { class TempAllocatorImpl; }
namespace JPH { class JobSystemThreadPool; }
namespace JPH { class PhysicsSystem; }
namespace JPH { class BodyInterface; }
#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#endif // WV_SUPPORT_JOLT_PHYSICS

#include <wv/Physics/BroadPhaseLayer.h>
#include <wv/Physics/PhysicsBodyDescriptor.h>

#include <wv/Types.h>
#include <wv/Math/Transform.h>

#include <unordered_map>

namespace wv
{
	#ifdef WV_SUPPORT_JOLT_PHYSICS
	class JoltContactListener;
	class JoltBodyActivationListener;
	#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////

	class cJoltPhysicsEngine
	{

	public:

		 cJoltPhysicsEngine() { }
		~cJoltPhysicsEngine() { }

		void init();
		void terminate();

		void killAllPhysicsBodies();
		void destroyPhysicsBody( const wv::Handle& _handle );

		void update( double _deltaTime );

		wv::Handle createAndAddBody( iPhysicsBodyDesc* _desc, bool _activate );
		
		Transformf getPhysicsBodyTransform( wv::Handle _handle );

		void setPhysicsBodyTransform( const wv::Handle& _handle, const Transformf& _transform );

///////////////////////////////////////////////////////////////////////////////////////

	private:

		const unsigned int m_maxBodies             = 1024; // 65536;
		const unsigned int m_numBodyMutexes        = 0;
		const unsigned int m_maxBodyPairs          = 1024; // 65536;
		const unsigned int m_maxContactConstraints = 1024; // 10240;

		const float  m_timestep    = 1.0f / 120.0f;
		float        m_accumulator = 0.0f;
		unsigned int m_steps       = 0;

	#ifdef WV_SUPPORT_JOLT_PHYSICS
		JPH::TempAllocatorImpl*   m_pTempAllocator = nullptr;
		JPH::JobSystemThreadPool* m_pJobSystem     = nullptr;
		JPH::PhysicsSystem*       m_pPhysicsSystem = nullptr;
		JPH::BodyInterface*       m_pBodyInterface = nullptr;

		cBroadPhaseLayer*               m_pBroadPhaseLayer               = nullptr;
		cObjectVsBroadPhaseLayerFilter* m_pObjectVsBroadPhaseLayerFilter = nullptr;
		cObjectLayerPairFilter*         m_pObjectLayerPairFilter         = nullptr;

		JoltContactListener*        tempContactListener        = nullptr;
		JoltBodyActivationListener* tempBodyActivationListener = nullptr;

		std::unordered_map<wv::Handle, JPH::BodyID> m_bodies;
	#endif // WV_SUPPORT_JOLT_PHYSICS

		wv::Handle m_cameraCollider = 0;
	};
}