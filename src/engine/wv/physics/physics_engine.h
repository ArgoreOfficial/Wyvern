#pragma once

///////////////////////////////////////////////////////////////////////////////////////

#include <wv/types.h>

#include <wv/math/transform.h>

#include <wv/physics/physics_types.h>

#include <unordered_map>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_JOLT_PHYSICS
namespace JPH { class TempAllocatorImpl; }
namespace JPH { class JobSystemThreadPool; }
namespace JPH { class PhysicsSystem; }
namespace JPH { class BodyInterface; }
namespace JPH { class Body; }

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	#ifdef WV_SUPPORT_JOLT_PHYSICS
	class JoltContactListener;
	class JoltBodyActivationListener;
	
	class BroadPhaseLayer;
	class ObjectVsBroadPhaseLayerFilter;
	class ObjectLayerPairFilter;
	#endif // WV_SUPPORT_JOLT_PHYSICS

	struct IPhysicsBodyDesc;

///////////////////////////////////////////////////////////////////////////////////////

	class JoltPhysicsEngine
	{
	public:
		 JoltPhysicsEngine() { }
		~JoltPhysicsEngine() { }

		void init();
		void terminate();

		void killAllPhysicsBodies();
		void destroyPhysicsBody( PhysicsBodyID _handle );

		void update( double _deltaTime );

		PhysicsBodyID createAndAddBody( IPhysicsBodyDesc* _desc, bool _activate );
		
		Transformf getBodyTransform      ( PhysicsBodyID& _handle );
		Vector3f   getBodyVelocity       ( PhysicsBodyID& _handle );
		Vector3f   getBodyAngularVelocity( PhysicsBodyID& _handle );

		bool isBodyActive( PhysicsBodyID& _handle );

		void setBodyTransform      ( PhysicsBodyID& _handle, const Transformf& _transform );
		void setBodyVelocity       ( PhysicsBodyID& _handle, const Vector3f& _velocity );
		void setBodyAngularVelocity( PhysicsBodyID& _handle, const Vector3f& _angularVelocity );

		void setBodyActive( PhysicsBodyID& _handle, bool _active );

///////////////////////////////////////////////////////////////////////////////////////

	private:
		static constexpr uint32_t k_tempAllocSize         = 64 * 1024 * 1024;
		static constexpr uint32_t k_maxBodies             = 16384;
		static constexpr uint32_t k_numBodyMutexes        = 0;
		static constexpr uint32_t k_maxBodyPairs          = k_maxBodies;
		static constexpr uint32_t k_maxContactConstraints = k_maxBodies;

		const float  m_timestep    = 1.0f / 120.0f;
		float        m_accumulator = 0.0f;
		unsigned int m_steps       = 0;

	#ifdef WV_SUPPORT_JOLT_PHYSICS
		JPH::TempAllocatorImpl*   m_pTempAllocator    = nullptr;
		JPH::JobSystemThreadPool* m_pPhysicsJobSystem = nullptr;
		JPH::PhysicsSystem*       m_pPhysicsSystem    = nullptr;
		
		BroadPhaseLayer*               m_pBroadPhaseLayer               = nullptr;
		ObjectVsBroadPhaseLayerFilter* m_pObjectVsBroadPhaseLayerFilter = nullptr;
		ObjectLayerPairFilter*         m_pObjectLayerPairFilter         = nullptr;

		JoltContactListener*        tempContactListener        = nullptr;
		JoltBodyActivationListener* tempBodyActivationListener = nullptr;

		std::unordered_map<wv::PhysicsBodyID, JPH::BodyID> m_bodies;
		std::mutex m_mutex;
	#endif // WV_SUPPORT_JOLT_PHYSICS

	};
}