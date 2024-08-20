#include "PhysicsEngine.h"

///////////////////////////////////////////////////////////////////////////////////////

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

#include <wv/Engine/Engine.h>
#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <stdarg.h>
#include <cstdarg>
#include <iostream>

JPH_SUPPRESS_WARNINGS

///////////////////////////////////////////////////////////////////////////////////////

/// TODO: move

class MyContactListener : public JPH::ContactListener
{
public:
	// See: ContactListener
	virtual JPH::ValidateResult OnContactValidate( const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult ) override
	{
		printf( "Contact validate callback\n" );

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings ) override
	{
		printf( "A contact was added\n" );
	}

	virtual void OnContactPersisted( const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings ) override
	{
		//printf( "A contact was persisted\n" );
	}

	virtual void OnContactRemoved( const JPH::SubShapeIDPair& inSubShapePair ) override
	{
		printf( "A contact was removed\n" );
	}
};

class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
	virtual void OnBodyActivated( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData ) override
	{
		printf( "A body got activated\n" );
	}

	virtual void OnBodyDeactivated( const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData ) override
	{
		printf( "A body went to sleep\n" );
	}
};

///////////////////////////////////////////////////////////////////////////////////////

static void traceImpl( const char* _msg, ... )
{
	/// TODO: change to wv::Debug::Print()
	va_list list;
	va_start( list, _msg );
	vprintf( _msg, list );
	printf( "\n" );
	va_end( list );
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl( const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine )
{
	// Print to the TTY
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << ( inMessage != nullptr ? inMessage : "" ) << std::endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::init()
{
	using namespace JPH::literals;

	JPH::RegisterDefaultAllocator();
	
	JPH::Trace = traceImpl;
	JPH_IF_ENABLE_ASSERTS( JPH::AssertFailed = AssertFailedImpl; )

	JPH::Factory::sInstance = new JPH::Factory();

	JPH::RegisterTypes();

	m_pTempAllocator = new JPH::TempAllocatorImpl( 10 * 1024 * 1024 );
	m_pJobSystem     = new JPH::JobSystemThreadPool( JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );

	m_pBroadPhaseLayer               = new cBroadPhaseLayer();
	m_pObjectVsBroadPhaseLayerFilter = new cObjectVsBroadPhaseLayerFilter();
	m_pObjectLayerPairFilter         = new cObjectLayerPairFilter();

	m_pPhysicsSystem = new JPH::PhysicsSystem();
	m_pPhysicsSystem->Init(
		m_maxBodies, m_numBodyMutexes, m_maxBodyPairs, m_maxContactConstraints,
		*m_pBroadPhaseLayer, 
		*m_pObjectVsBroadPhaseLayerFilter, 
		*m_pObjectLayerPairFilter );

	tempContactListener = new MyContactListener();
	tempBodyActivationListener = new MyBodyActivationListener();

	m_pPhysicsSystem->SetContactListener( tempContactListener );
	m_pPhysicsSystem->SetBodyActivationListener( tempBodyActivationListener );

	m_pBodyInterface = &m_pPhysicsSystem->GetBodyInterface();

	JPH::BoxShapeSettings floorShapeSettings( JPH::Vec3( 100.0f, 1.0f, 100.0f ) );
	floorShapeSettings.SetEmbedded();

	JPH::ShapeSettings::ShapeResult floorShapeResult = floorShapeSettings.Create();
	JPH::ShapeRefC floorShape = floorShapeResult.Get();

	JPH::BodyCreationSettings floorSettings( 
		floorShape, 
		JPH::RVec3( 0.0_r, -1.0_r, 0.0_r ), 
		JPH::Quat::sIdentity(), 
		JPH::EMotionType::Static, 
		Layers::STATIC 
	);

	createAndAddBody( floorSettings, false );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::shutdown()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::update( double _deltaTime )
{
	// Next step
	m_steps++;

	/// TODO: discrete
	// 1 collision step per 1 / 60th of a second (round up).
	const int collisionSteps = 1;

	// Step the world
	m_pPhysicsSystem->Update( _deltaTime, collisionSteps, m_pTempAllocator, m_pJobSystem );
}

wv::Handle wv::cJoltPhysicsEngine::createAndAddBody( const JPH::BodyCreationSettings& _settings, bool _activate )
{
	JPH::Body* body = m_pBodyInterface->CreateBody( _settings );
	JPH::BodyID id = body->GetID();
	wv::Handle handle = -1;

	m_pBodyInterface->AddBody( id, _activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate );

	do { handle = cEngine::getUniqueHandle(); } 
	while( m_bodies.contains( handle ) );

	m_bodies[ handle ] = id;
	return handle;
}

wv::Transformf wv::cJoltPhysicsEngine::getPhysicsBodyTransform( wv::Handle _handle )
{
	JPH::BodyID id = m_bodies.at( _handle );

	JPH::RVec3 pos = m_pBodyInterface->GetCenterOfMassPosition( id );
	JPH::Vec3  rot = m_pBodyInterface->GetRotation( id ).GetEulerAngles(); /// TODO: change to quaternion

	Transformf transform{};
	transform.position = Vector3f{ pos.GetX(), pos.GetY(), pos.GetZ() };
	transform.rotation = Vector3f{ 
		wv::Math::radToDeg( rot.GetX() ), 
		wv::Math::radToDeg( rot.GetY() ), 
		wv::Math::radToDeg( rot.GetZ() ) 
	};

	return transform;
}
