#include "PhysicsEngine.h"

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_JOLT_PHYSICS
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
#endif // WV_SUPPORT_JOLT_PHYSICS

#include <wv/Engine/Engine.h>
#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <wv/Camera/Camera.h>

#include <stdarg.h>
#include <cstdarg>
#include <iostream>
#ifdef WV_SUPPORT_JOLT_PHYSICS
JPH_SUPPRESS_WARNINGS
#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////

/// TODO: move
#ifdef WV_SUPPORT_JOLT_PHYSICS
class MyContactListener : public JPH::ContactListener
{
public:
	// See: ContactListener
	virtual JPH::ValidateResult OnContactValidate( const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult ) override
	{
		// printf( "Contact validate callback\n" );

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
#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_JOLT_PHYSICS
static wv::Vector3f JPHtoWV( const JPH::Vec3& _vec )
{
	return { _vec.GetX(), _vec.GetY(), _vec.GetZ() };
}

static JPH::Vec3 WVtoJPH( const wv::Vector3f& _vec )
{
	return { _vec.x, _vec.y, _vec.z };
}
#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_JOLT_PHYSICS
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
#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::init()
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
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


	sPhysicsSphereDesc* ballDesc = new sPhysicsSphereDesc();
	ballDesc->kind = WV_PHYSICS_KINEMATIC;
	ballDesc->radius = 1.0f;
	m_cameraCollider = createAndAddBody( ballDesc, true );
#else
	wv::Debug::Print( Debug::WV_PRINT_ERROR, "Jolt Physics is not supported on this platform\n" );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::terminate()
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	// Unregisters all types with the factory and cleans up the default material
	JPH::UnregisterTypes();

	// Destroy the factory
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
#endif // WV_SUPPORT_JOLT_PHYSICS
}

void wv::cJoltPhysicsEngine::killAllPhysicsBodies()
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	for( auto& body : m_bodies )
	{
		m_pBodyInterface->RemoveBody( body.second );
		m_pBodyInterface->DestroyBody( body.second );
	}

	m_bodies.clear();
#endif // WV_SUPPORT_JOLT_PHYSICS
}

void wv::cJoltPhysicsEngine::destroyPhysicsBody( const wv::Handle& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	if( !m_bodies.contains( _handle ) )
	{
		wv::Debug::Print( Debug::WV_PRINT_ERROR, "No Physics Body with Handle %i\n", _handle );
		return;
	}

	JPH::BodyID id = m_bodies.at( _handle );
	m_pBodyInterface->RemoveBody( id );
	m_pBodyInterface->DestroyBody( id );
	m_bodies.erase( _handle );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::update( double _deltaTime )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	cEngine* app = cEngine::get();
	if( app->currentCamera )
		setPhysicsBodyTransform( m_cameraCollider, app->currentCamera->getTransform() );
	
	// physics update
	float frameTime = _deltaTime;
	if( frameTime > 0.016f )
		frameTime = 0.016f;

	// Next step
	m_steps++;

	int collisionSteps = 0;
	
	m_accumulator += frameTime;
	while( m_accumulator >= m_timestep )
	{
		collisionSteps++;
		m_accumulator -= m_timestep;
	}

	if( collisionSteps > 0 )
		m_pPhysicsSystem->Update( m_timestep, collisionSteps, m_pTempAllocator, m_pJobSystem );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Handle wv::cJoltPhysicsEngine::createAndAddBody( iPhysicsBodyDesc* _desc, bool _activate )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::Shape* shape = nullptr;
	JPH::RVec3 pos = WVtoJPH( _desc->transform.position );
	JPH::RVec3 rot = WVtoJPH( _desc->transform.rotation );
	
	switch( _desc->shape )
	{
	case WV_PHYSICS_BOX:
	{
		sPhysicsBoxDesc* desc = static_cast< sPhysicsBoxDesc* >( _desc );
		JPH::RVec3 halfExtent = WVtoJPH( desc->halfExtent );
		shape = new JPH::BoxShape( halfExtent );
	} break;

	case WV_PHYSICS_SPHERE:
	{
		sPhysicsSphereDesc* desc = static_cast< sPhysicsSphereDesc* >( _desc );
		shape = new JPH::SphereShape( desc->radius );
	} break;

	default: Debug::Print( Debug::WV_PRINT_ERROR, "Physics shape unimplemented" ); break;
	}

	if( shape == nullptr )
		return 0;
	
	JPH::EMotionType motionType = JPH::EMotionType::Static;
	switch( _desc->kind )
	{
	case WV_PHYSICS_DYANIMIC:  motionType = JPH::EMotionType::Dynamic;   break;
	case WV_PHYSICS_KINEMATIC: motionType = JPH::EMotionType::Kinematic; break;
	}

	const JPH::ObjectLayer layer = _desc->kind == WV_PHYSICS_STATIC ? wv::Layers::STATIC : wv::Layers::DYNAMIC;


	JPH::BodyCreationSettings settings( shape, pos, JPH::Quat::sEulerAngles( rot ), motionType, layer );

	JPH::Body*  body   = m_pBodyInterface->CreateBody( settings );
	JPH::BodyID id     = body->GetID();
	wv::Handle  handle = 0;

	m_pBodyInterface->AddBody( id, _activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate );
	m_pBodyInterface->SetAngularVelocity( id, { 0.0f, 25.0f, 0.0f } );
	
	do { handle = cEngine::getUniqueHandle(); } while( m_bodies.contains( handle ) );

	m_bodies[ handle ] = id;
	return handle;
#else
	return 0;
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Transformf wv::cJoltPhysicsEngine::getPhysicsBodyTransform( wv::Handle _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
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
#else
	return {};
#endif // WV_SUPPORT_JOLT_PHYSICS
}

void wv::cJoltPhysicsEngine::setPhysicsBodyTransform( const wv::Handle& _handle, const Transformf& _transform )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyID id = m_bodies.at( _handle );

	JPH::Vec3 pos = WVtoJPH( _transform.position );
	JPH::Vec3 rot = WVtoJPH( _transform.rotation );
	
	m_pBodyInterface->SetPositionAndRotation( id, pos, JPH::Quat::sEulerAngles( rot ), JPH::EActivation::DontActivate);
#endif // WV_SUPPORT_JOLT_PHYSICS
}