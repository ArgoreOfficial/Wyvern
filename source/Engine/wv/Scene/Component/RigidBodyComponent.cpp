#include "RigidBodyComponent.h"

#include <wv/Engine/Engine.h>
#include <wv/Memory/Memory.h>

#include <wv/Physics/PhysicsEngine.h>
#include <wv/Physics/PhysicsBodyDescriptor.h>

#include <wv/Scene/Entity/Entity.h>

void wv::RigidBodyComponent::onEnter( void )
{
	cEngine* app = wv::cEngine::get();

	//sphereSettings.mLinearVelocity = JPH::Vec3( 1.0f, 10.0f, 2.0f );
	//sphereSettings.mRestitution = 0.4f;
#ifdef WV_SUPPORT_PHYSICS
	Entity* parent = getParent();
	m_pPhysicsBodyDesc->transform = parent->m_transform;
	m_physicsBodyHandle = app->m_pPhysicsEngine->createAndAddBody( m_pPhysicsBodyDesc, true );
#endif // WV_SUPPORT_PHYSICS

	WV_FREE( m_pPhysicsBodyDesc );
	m_pPhysicsBodyDesc = nullptr;
}

void wv::RigidBodyComponent::onExit( void )
{
	wv::cEngine* app = wv::cEngine::get();

	app->m_pPhysicsEngine->destroyPhysicsBody( m_physicsBodyHandle );
	m_physicsBodyHandle = PhysicsBodyID::InvalidID;
}

void wv::RigidBodyComponent::onPhysicsUpdate( double _deltaTime )
{
	wv::cJoltPhysicsEngine* pPhysics = wv::cEngine::get()->m_pPhysicsEngine;

	if ( !m_physicsBodyHandle.is_valid() || !pPhysics->isBodyActive( m_physicsBodyHandle ) )
		return;

	Transformf t = pPhysics->getBodyTransform( m_physicsBodyHandle );
	getParent()->m_transform.setPositionRotation( t.position, t.rotation );
}
