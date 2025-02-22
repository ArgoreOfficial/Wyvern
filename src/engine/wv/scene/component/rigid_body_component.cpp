#include "rigid_body_component.h"

#include <wv/engine.h>
#include <wv/memory/memory.h>

#include <wv/physics/physics_engine.h>
#include <wv/physics/physics_body_descriptor.h>

#include <wv/Scene/Entity/Entity.h>

void wv::RigidBodyComponent::onEnter( void )
{
	//sphereSettings.mLinearVelocity = JPH::Vec3( 1.0f, 10.0f, 2.0f );
	//sphereSettings.mRestitution = 0.4f;

#ifdef WV_SUPPORT_PHYSICS
	Engine* app = wv::Engine::get();
	Entity* parent = getParent();
	m_pPhysicsBodyDesc->transform = parent->m_transform;
	m_physicsBodyHandle = app->m_pPhysicsEngine->createAndAddBody( m_pPhysicsBodyDesc, true );
#endif // WV_SUPPORT_PHYSICS

	WV_FREE( m_pPhysicsBodyDesc );
	m_pPhysicsBodyDesc = nullptr;
}

void wv::RigidBodyComponent::onExit( void )
{
	wv::Engine* app = wv::Engine::get();

	app->m_pPhysicsEngine->destroyPhysicsBody( m_physicsBodyHandle );
	m_physicsBodyHandle = PhysicsBodyID::InvalidID;
}

void wv::RigidBodyComponent::onPhysicsUpdate( double /*_deltaTime*/ )
{
	wv::JoltPhysicsEngine* pPhysics = wv::Engine::get()->m_pPhysicsEngine;

	if ( !m_physicsBodyHandle.is_valid() || !pPhysics->isBodyActive( m_physicsBodyHandle ) )
		return;

	Transformf t = pPhysics->getBodyTransform( m_physicsBodyHandle );
	getParent()->m_transform.setPositionRotation( t.position, t.rotation );
}
