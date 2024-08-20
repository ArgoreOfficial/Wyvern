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

#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <wv/Physics/BroadPhaseLayer.h>

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
		printf( "A contact was persisted\n" );
	}

	virtual void OnContactRemoved( const JPH::SubShapeIDPair& inSubShapePair ) override
	{
		printf( "A contact was removed" );
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
		Layers::STATIC );

	// Create the actual rigid body
	JPH::Body* floor = m_pBodyInterface->CreateBody( floorSettings );

	// Add it to the world
	m_pBodyInterface->AddBody( floor->GetID(), JPH::EActivation::DontActivate );

	// Now create a dynamic body to bounce on the floor
	// Note that this uses the shorthand version of creating and adding a body to the world
	JPH::BodyCreationSettings sphereSettings( new JPH::SphereShape( 0.5f ), JPH::RVec3( 0.0_r, 20.0_r, 0.0_r ), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::DYNAMIC );
	sphereSettings.mRestitution = 0.8f;
	sphereID = m_pBodyInterface->CreateAndAddBody( sphereSettings, JPH::EActivation::Activate );
	// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
	// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
	m_pBodyInterface->SetLinearVelocity( sphereID, JPH::Vec3( 0.0f, 10.0f, 0.0f ) );
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

	// Output current position and velocity of the sphere
	JPH::RVec3 position = m_pBodyInterface->GetCenterOfMassPosition( sphereID );
	JPH::Vec3 velocity = m_pBodyInterface->GetLinearVelocity( sphereID );
	
	wv::Debug::Draw::AddSphere( { position.GetX(), position.GetY(), position.GetZ() } );

	/// TODO: discrete
	// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
	const int collisionSteps = 1;

	// Step the world
	m_pPhysicsSystem->Update( _deltaTime, collisionSteps, m_pTempAllocator, m_pJobSystem );
}
