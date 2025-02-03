#include "PhysicsEngine.h"

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Physics/Constraints/PulleyConstraint.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>

#include <wv/Physics/BroadPhaseLayer.h>
#include <wv/Physics/PhysicsBodyDescriptor.h>

#endif // WV_SUPPORT_JOLT_PHYSICS

#include <wv/Engine/Engine.h>
#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <wv/Camera/Camera.h>
#include <wv/Physics/PhysicsListeners.h>
#include <wv/Memory/Memory.h>

#include <stdarg.h>
#include <cstdarg>
#include <iostream>

#ifdef WV_SUPPORT_JOLT_PHYSICS
JPH_SUPPRESS_WARNINGS
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

	JPH::Factory::sInstance = WV_NEW( JPH::Factory );

	JPH::RegisterTypes();

	m_pTempAllocator    = WV_NEW( JPH::TempAllocatorImpl, 10 * 1024 * 1024 );
	m_pPhysicsJobSystem = WV_NEW( JPH::JobSystemThreadPool, JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );

	m_pBroadPhaseLayer               = WV_NEW( cBroadPhaseLayer );
	m_pObjectVsBroadPhaseLayerFilter = WV_NEW( cObjectVsBroadPhaseLayerFilter );
	m_pObjectLayerPairFilter         = WV_NEW( cObjectLayerPairFilter );

	m_pPhysicsSystem = WV_NEW( JPH::PhysicsSystem );
	m_pPhysicsSystem->Init(
		m_maxBodies, m_numBodyMutexes, m_maxBodyPairs, m_maxContactConstraints,
		*m_pBroadPhaseLayer, 
		*m_pObjectVsBroadPhaseLayerFilter, 
		*m_pObjectLayerPairFilter );

	tempContactListener        = WV_NEW( cJoltContactListener );
	tempBodyActivationListener = WV_NEW( cJoltBodyActivationListener );

	m_pPhysicsSystem->SetContactListener( tempContactListener );
	m_pPhysicsSystem->SetBodyActivationListener( tempBodyActivationListener );

#else
	wv::Debug::Print( Debug::WV_PRINT_ERROR, "Jolt Physics is not supported on this platform\n" );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::terminate()
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::UnregisterTypes();

	WV_FREE( tempBodyActivationListener );
	WV_FREE( tempContactListener );
	WV_FREE( m_pPhysicsSystem );
	WV_FREE( m_pObjectLayerPairFilter );
	WV_FREE( m_pObjectVsBroadPhaseLayerFilter );
	WV_FREE( m_pBroadPhaseLayer );
	WV_FREE( m_pPhysicsJobSystem );
	WV_FREE( m_pTempAllocator );

	// Destroy the factory
	WV_FREE( JPH::Factory::sInstance );
	JPH::Factory::sInstance = nullptr;
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::killAllPhysicsBodies()
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	std::scoped_lock lock{ m_mutex };
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();

	for( auto& body : m_bodies )
	{
		bodyInterface.RemoveBody ( body.second );
		bodyInterface.DestroyBody( body.second );
	}

	m_bodies.clear();
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::destroyPhysicsBody( PhysicsBodyID _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	std::scoped_lock lock{ m_mutex };

	if( !m_bodies.contains( _handle ) )
	{
		wv::Debug::Print( Debug::WV_PRINT_ERROR, "No Physics Body with Handle %i\n", _handle );
		return;
	}

	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );

	bodyInterface.RemoveBody ( body );
	bodyInterface.DestroyBody( body );
	m_bodies.erase( _handle );
	_handle.invalidate();
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::update( double _deltaTime )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	// physics update
	float frameTime = wv::Math::min( _deltaTime, 0.05 );
	
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
		m_pPhysicsSystem->Update( wv::Math::max( (float)_deltaTime, m_timestep ), collisionSteps, m_pTempAllocator, m_pPhysicsJobSystem );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PhysicsBodyID wv::cJoltPhysicsEngine::createAndAddBody( iPhysicsBodyDesc* _desc, bool _activate )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	std::scoped_lock lock{ m_mutex };

	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();

	JPH::Shape* shape = nullptr;
	JPH::RVec3 pos = WVtoJPH( _desc->transform.position );
	JPH::RVec3 rot = WVtoJPH( _desc->transform.rotation );
	
	switch( _desc->shape )
	{
	case WV_PHYSICS_BOX:
	{
		sPhysicsBoxDesc* desc = static_cast< sPhysicsBoxDesc* >( _desc );
		JPH::RVec3 halfExtent = WVtoJPH( desc->halfExtent );
		shape = WV_NEW( JPH::BoxShape, halfExtent );
	} break;

	case WV_PHYSICS_SPHERE:
	{
		sPhysicsSphereDesc* desc = static_cast< sPhysicsSphereDesc* >( _desc );
		shape = WV_NEW( JPH::SphereShape, desc->radius );
	} break;

	default: Debug::Print( Debug::WV_PRINT_ERROR, "Physics shape unimplemented" ); break;
	}

	if( shape == nullptr )
		return { PhysicsBodyID::InvalidID };
	
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

	WV_RELINQUISH( shape ); // body interface takes ownership of shape 
	JPH::Body* body = bodyInterface.CreateBody( settings );
	if( !body )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Too many Rigidbodies!\n" );
		return { PhysicsBodyID::InvalidID };
	}

	JPH::BodyID id = body->GetID();
	wv::PhysicsBodyID handle{ 0 };

	bodyInterface.AddBody( id, _activate ? JPH::EActivation::Activate : JPH::EActivation::DontActivate );

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

	do 
	{
		handle.value = cEngine::getUniqueHandle(); 
	} while( m_bodies.contains( handle ) );

	m_bodies[ handle ] = id;
	return handle;
#else
	return 0;
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Transformf wv::cJoltPhysicsEngine::getBodyTransform( PhysicsBodyID& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	//std::scoped_lock lock{ m_mutex };

	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );

	JPH::RVec3 pos = bodyInterface.GetPosition( body );
	JPH::Vec3  rot = bodyInterface.GetRotation( body ).GetEulerAngles(); /// TODO: change to quaternion

	Transformf transform{};
	transform.position = Vector3f{ pos.GetX(), pos.GetY(), pos.GetZ() };
	transform.rotation = Vector3f{ 
		wv::Math::degrees( rot.GetX() ), 
		wv::Math::degrees( rot.GetY() ), 
		wv::Math::degrees( rot.GetZ() ) 
	};

	/// TODO: body->GetWorldTransform to transform.matrix

	return transform;
#else
	return {};
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::cJoltPhysicsEngine::getBodyVelocity( PhysicsBodyID& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );

	return JPHtoWV( bodyInterface.GetLinearVelocity( body ) );
#else
	return {};
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::cJoltPhysicsEngine::getBodyAngularVelocity( PhysicsBodyID& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );


	return JPHtoWV( bodyInterface.GetAngularVelocity( body ) );
#else
	return {};
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cJoltPhysicsEngine::isBodyActive( PhysicsBodyID& _handle )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	return bodyInterface.IsActive( m_bodies.at( _handle ) );
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::setBodyTransform( PhysicsBodyID& _handle, const Transformf& _transform )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );

	JPH::Vec3 pos = WVtoJPH( _transform.position );
	JPH::Vec3 rot{
		wv::Math::radians( _transform.rotation.x ),
		wv::Math::radians( _transform.rotation.y ),
		wv::Math::radians( _transform.rotation.z )
	};
	
	bodyInterface.SetPositionAndRotation( body, pos, JPH::Quat::sEulerAngles( rot ), JPH::EActivation::DontActivate );
#endif // WV_SUPPORT_JOLT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::setBodyVelocity( PhysicsBodyID& _handle, const Vector3f& _velocity )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );
	JPH::Vec3 vel = WVtoJPH( _velocity );
	
	bodyInterface.SetLinearVelocity( body, WVtoJPH( _velocity ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::setBodyAngularVelocity( PhysicsBodyID& _handle, const Vector3f& _angularVelocity )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );
	JPH::Vec3 vel = WVtoJPH( _angularVelocity );

	bodyInterface.SetAngularVelocity( body, vel );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cJoltPhysicsEngine::setBodyActive( PhysicsBodyID& _handle, bool _active )
{
#ifdef WV_SUPPORT_JOLT_PHYSICS
	JPH::BodyInterface& bodyInterface = m_pPhysicsSystem->GetBodyInterface();
	JPH::BodyID body = m_bodies.at( _handle );
	
	if( _active )
		bodyInterface.ActivateBody( body );
	else
		bodyInterface.DeactivateBody( body );
#endif
}
