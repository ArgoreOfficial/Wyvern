#pragma once

///////////////////////////////////////////////////////////////////////////////////////

#include <wv/Types.h>

#include <wv/Math/Transform.h>

#include <wv/Physics/PhysicsTypes.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_JOLT_PHYSICS
namespace JPH { class TempAllocatorImpl; }
namespace JPH { class JobSystemThreadPool; }
namespace JPH { class PhysicsSystem; }
namespace JPH { class BodyInterface; }
namespace JPH { class Body; }
#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	#ifdef WV_SUPPORT_JOLT_PHYSICS
	class cJoltContactListener;
	class cJoltBodyActivationListener;
	
	class cBroadPhaseLayer;
	class cObjectVsBroadPhaseLayerFilter;
	class cObjectLayerPairFilter;
	#endif // WV_SUPPORT_JOLT_PHYSICS

	struct iPhysicsBodyDesc;

///////////////////////////////////////////////////////////////////////////////////////

	class cJoltPhysicsEngine
	{
	public:
		 cJoltPhysicsEngine() { }
		~cJoltPhysicsEngine() { }

		void init();
		void terminate();

		void killAllPhysicsBodies();
		void destroyPhysicsBody( PhysicsBodyID& _handle );

		void update( double _deltaTime );

		PhysicsBodyID createAndAddBody( iPhysicsBodyDesc* _desc, bool _activate );
		
		Transformf getBodyTransform      ( PhysicsBodyID& _handle );
		Vector3f  getBodyVelocity       ( PhysicsBodyID& _handle );
		Vector3f  getBodyAngularVelocity( PhysicsBodyID& _handle );

		bool isBodyActive( PhysicsBodyID& _handle );

		void setBodyTransform      ( PhysicsBodyID& _handle, const Transformf& _transform );
		void setBodyVelocity       ( PhysicsBodyID& _handle, const Vector3f& _velocity );
		void setBodyAngularVelocity( PhysicsBodyID& _handle, const Vector3f& _angularVelocity );

		void setBodyActive( PhysicsBodyID& _handle, bool _active );

///////////////////////////////////////////////////////////////////////////////////////

	private:
		const unsigned int m_maxBodies             = 65536;
		const unsigned int m_numBodyMutexes        = 0;
		const unsigned int m_maxBodyPairs          = 65536;
		const unsigned int m_maxContactConstraints = 10240;

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

		cJoltContactListener*        tempContactListener        = nullptr;
		cJoltBodyActivationListener* tempBodyActivationListener = nullptr;

		std::unordered_map<wv::PhysicsBodyID, JPH::Body*> m_bodies;
	#endif // WV_SUPPORT_JOLT_PHYSICS

	};
}