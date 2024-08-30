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

#include <Jolt/Physics/Constraints/PulleyConstraint.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#endif // WV_SUPPORT_JOLT_PHYSICS

#include <wv/Engine/Engine.h>
#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <wv/Camera/Camera.h>
#include <wv/Physics/PhysicsListeners.h>

#include <stdarg.h>
#include <cstdarg>
#include <iostream>

#ifdef WV_SUPPORT_JOLT_PHYSICS
JPH_SUPPRESS_WARNINGS
#endif // WV_SUPPORT_JOLT_PHYSICS

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_JOLT_PHYSICS
static wv::cVector3f JPHtoWV( const JPH::Vec3& _vec )
{
	return { _vec.GetX(), _vec.GetY(), _vec.GetZ() };
}

static JPH::Vec3 WVtoJPH( const wv::cVector3f& _vec )
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

///////////////////////////////////////////////////////////////////////////////////////

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

	tempContactListener = new cJoltContactListener();
	tempBodyActivationListener = new cJoltBodyActivationListener();

	m_pPhysicsSystem->SetContactListener( tempContactListener );
	m_pPhysicsSystem->SetBodyActivationListener( tempBodyActivationListener );

	m_pBodyInterface = &m_pPhysicsSystem->GetBodyInterface();


	sPhysicsSphereDesc* ballDesc = new sPhysicsSphereDesc();
	ballDesc->kind = WV_PHYSICS_KINEMATIC;
	ballDesc->radius = 1.0f;

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

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::killAllPhysicsBodies()
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	for( auto& body : m_bodies )
	{
		m_pBodyInterface->RemoveBody ( body.second->GetID() );
		m_pBodyInterface->DestroyBody( body.second->GetID() );
	}

	m_bodies.clear();
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::destroyPhysicsBody( hPhysicsBody& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	if( !m_bodies.contains( _handle.value() ) )
	{
		wv::Debug::Print( Debug::WV_PRINT_ERROR, "No Physics Body with Handle %i\n", _handle );
		return;
	}

	JPH::Body* body = m_bodies.at( _handle.value() );
	m_pBodyInterface->RemoveBody ( body->GetID() );
	m_pBodyInterface->DestroyBody( body->GetID() );
	m_bodies.erase( _handle.value() );
	_handle.invalidate();
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::update( double _deltaTime )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
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
		m_pPhysicsSystem->Update( m_timestep * (float)collisionSteps, collisionSteps, m_pTempAllocator, m_pJobSystem );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

wv::hPhysicsBody wv::cJoltPhysicsEngine::createAndAddBody( iPhysicsBodyDesc* _desc, bool _activate )
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
	settings.mFriction = 0.5f;
	settings.mRestitution = 0.5f;

	JPH::Body*  body   = m_pBodyInterface->CreateBody( settings );
	if( !body )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Too many Rigidbodies!\n" );
		return 0;
	}

	JPH::BodyID id     = body->GetID();
	wv::Handle  handle = 0;

	m_pBodyInterface->AddBody( id, _activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate );
	m_pBodyInterface->SetAngularVelocity( id, { 4.0f, 0.0f, 0.0f } );
	
	// setup constraint
	/*
	if( m_cameraCollider != 0 )
	{
		JPH::DistanceConstraintSettings distanceSetting;
		distanceSetting.mPoint1 = body->GetPosition();
		distanceSetting.mPoint2 = m_bodies[ m_cameraCollider ]->GetPosition();
		distanceSetting.mMinDistance = 0;
		distanceSetting.mMaxDistance = 10;
		m_pPhysicsSystem->AddConstraint( distanceSetting.Create( *body, *m_bodies[ m_cameraCollider ] ) );
	}
	*/

	do { handle = cEngine::getUniqueHandle(); } while( m_bodies.contains( handle ) );

	m_bodies[ handle ] = body;
	return handle;
#else
	return 0;
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Transformf wv::cJoltPhysicsEngine::getBodyTransform( hPhysicsBody& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::Body* body = m_bodies.at( _handle.value() );
	
	JPH::RVec3 pos = body->GetPosition();
	JPH::Vec3  rot = body->GetRotation().GetEulerAngles(); /// TODO: change to quaternion

	Transformf transform{};
	transform.position = cVector3f{ pos.GetX(), pos.GetY(), pos.GetZ() };
	transform.rotation = cVector3f{ 
		wv::Math::degrees( rot.GetX() ), 
		wv::Math::degrees( rot.GetY() ), 
		wv::Math::degrees( rot.GetZ() ) 
	};

	return transform;
#else
	return {};
#endif // WV_SUPPORT_JOLT_PHYSICS
}

wv::cVector3f wv::cJoltPhysicsEngine::getBodyVelocity( hPhysicsBody& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::Body* body = m_bodies.at( _handle.value() );
	
	return JPHtoWV( body->GetLinearVelocity() );
#else
	return {};
#endif
}

wv::cVector3f wv::cJoltPhysicsEngine::getBodyAngularVelocity( hPhysicsBody& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::Body* body = m_bodies.at( _handle.value() );

	return JPHtoWV( body->GetAngularVelocity() );
#else
	return {};
#endif
}

bool wv::cJoltPhysicsEngine::isBodyActive( hPhysicsBody& _handle )
{
	return m_bodies.at( _handle.value() )->IsActive();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::setBodyTransform( hPhysicsBody& _handle, const Transformf& _transform )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::Body* body = m_bodies.at( _handle.value() );

	JPH::Vec3 pos = WVtoJPH( _transform.position );
	JPH::Vec3 rot{
		wv::Math::radians( _transform.rotation.x ),
		wv::Math::radians( _transform.rotation.y ),
		wv::Math::radians( _transform.rotation.z )
	};
	
	m_pBodyInterface->SetPositionAndRotation( body->GetID(), pos, JPH::Quat::sEulerAngles( rot ), JPH::EActivation::DontActivate );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::setBodyVelocity( hPhysicsBody& _handle, const cVector3f& _velocity )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::Body* body = m_bodies.at( _handle.value() );
	JPH::Vec3 vel = WVtoJPH( _velocity );
	
	body->SetLinearVelocity( WVtoJPH( _velocity ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::setBodyAngularVelocity( hPhysicsBody& _handle, const cVector3f& _angularVelocity )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::Body* body = m_bodies.at( _handle.value() );
	JPH::Vec3 vel = WVtoJPH( _angularVelocity );

	body->SetAngularVelocity( vel );
#endif
}

void wv::cJoltPhysicsEngine::setBodyActive( hPhysicsBody& _handle, bool _active )
{
	JPH::Body* body = m_bodies.at( _handle.value() );
	
	if( _active )
		m_pBodyInterface->ActivateBody( body->GetID() );
	else
		m_pBodyInterface->DeactivateBody( body->GetID() );
}
