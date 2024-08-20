#pragma once

///////////////////////////////////////////////////////////////////////////////////////

namespace JPH { class TempAllocatorImpl; }
namespace JPH { class JobSystemThreadPool; }
namespace JPH { class PhysicsSystem; }
namespace JPH { class BodyInterface; }

///////////////////////////////////////////////////////////////////////////////////////

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#include <wv/Types.h>
#include <wv/Math/Transform.h>

#include <unordered_map>

class MyContactListener;
class MyBodyActivationListener;

namespace wv
{

	class cBroadPhaseLayer;
	class cObjectVsBroadPhaseLayerFilter;
	class cObjectLayerPairFilter;

///////////////////////////////////////////////////////////////////////////////////////

	class cJoltPhysicsEngine
	{

	public:

		 cJoltPhysicsEngine() { }
		~cJoltPhysicsEngine() { }

		void init();
		void shutdown();

		void update( double _deltaTime );

		Transformf getPhysicsBodyTransform( wv::Handle _handle );

///////////////////////////////////////////////////////////////////////////////////////

	private:

		const unsigned int m_maxBodies             = 1024; // 65536;
		const unsigned int m_numBodyMutexes        = 0;
		const unsigned int m_maxBodyPairs          = 1024; // 65536;
		const unsigned int m_maxContactConstraints = 1024; // 10240;

		const float m_timestep = 1.0f / 60.0f;
		unsigned int m_steps = 0;

		JPH::TempAllocatorImpl*   m_pTempAllocator = nullptr;
		JPH::JobSystemThreadPool* m_pJobSystem     = nullptr;
		JPH::PhysicsSystem*       m_pPhysicsSystem = nullptr;
		JPH::BodyInterface*       m_pBodyInterface = nullptr;

		cBroadPhaseLayer*               m_pBroadPhaseLayer               = nullptr;
		cObjectVsBroadPhaseLayerFilter* m_pObjectVsBroadPhaseLayerFilter = nullptr;
		cObjectLayerPairFilter*         m_pObjectLayerPairFilter         = nullptr;

		MyContactListener* tempContactListener = nullptr;
		MyBodyActivationListener* tempBodyActivationListener = nullptr;

		std::unordered_map<wv::Handle, JPH::BodyID> m_bodies;

	};
}