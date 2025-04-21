#include "rigidbody.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/material/material.h>

#include <wv/memory/model_parser.h>
#include <wv/filesystem/file_system.h>

#include <wv/physics/physics_engine.h>
#include <wv/physics/physics_body_descriptor.h>

#include <wv/resource/resource_registry.h>

#include <wv/scene/component/model_component.h>
#include <wv/scene/component/rigid_body_component.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::Rigidbody::Rigidbody( const UUID& _uuid, const std::string& _name ) :
	Entity{ _uuid, _name }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::Rigidbody::Rigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, IPhysicsBodyDesc* _bodyDesc ) : 
	Entity{ _uuid, _name },
	m_meshPath{ _meshPath },
	m_pPhysicsBodyDesc{ _bodyDesc }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::Rigidbody::~Rigidbody()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Rigidbody::onConstruct()
{
#ifdef WV_SUPPORT_PHYSICS
	if ( !m_pPhysicsBodyDesc )
	{
		switch ( m_shape )
		{
		case WV_PHYSICS_NONE: 
			wv::Debug::Print( Debug::WV_PRINT_ERROR, "wv::Rigidbody->m_shape was PHYSICS_NONE\n" );
			break;

		case WV_PHYSICS_SPHERE:
		{
			PhysicsSphereDesc* sphereDesc = WV_NEW( PhysicsSphereDesc );
			sphereDesc->radius = m_sphereRadius;
			m_pPhysicsBodyDesc = sphereDesc;
		} break;

		case WV_PHYSICS_BOX:
		{
			PhysicsBoxDesc* boxDesc = WV_NEW( PhysicsBoxDesc );
			boxDesc->halfExtent = m_boxExtents;
			m_pPhysicsBodyDesc = boxDesc;
		} break;

		case WV_PHYSICS_CAPSULE:          break;
		case WV_PHYSICS_TAPERRED_CAPSULE: break;
		case WV_PHYSICS_CYLINDER:         break;
		case WV_PHYSICS_CONVECT_HULL:     break;
		case WV_PHYSICS_PLANE:            break;
		case WV_PHYSICS_MESH:             break;
		case WV_PHYSICS_TERRAIN:          break;

		}
	}

	if ( m_pPhysicsBodyDesc )
		m_pPhysicsBodyDesc->kind = (PhysicsKind)m_kind;
#endif

	addComponent<ModelComponent>( m_meshPath );
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Rigidbody::onDestruct()
{
	if ( m_pPhysicsBodyDesc )
	{
		WV_FREE( m_pPhysicsBodyDesc );
		m_pPhysicsBodyDesc = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Rigidbody::onEnter()
{
	//sphereSettings.mLinearVelocity = JPH::Vec3( 1.0f, 10.0f, 2.0f );
	//sphereSettings.mRestitution = 0.4f;

	Engine* app = wv::Engine::get();
	m_pPhysicsBodyDesc->transform = m_transform;
	m_physicsBodyHandle = app->m_pPhysicsEngine->createAndAddBody( m_pPhysicsBodyDesc, true );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Rigidbody::onExit()
{
	wv::Engine* app = wv::Engine::get();
	app->m_pPhysicsEngine->destroyPhysicsBody( m_physicsBodyHandle );
	m_physicsBodyHandle = PhysicsBodyID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Rigidbody::onPhysicsUpdate( double _dt )
{
	wv::JoltPhysicsEngine* pPhysics = wv::Engine::get()->m_pPhysicsEngine;
	if ( !m_physicsBodyHandle.is_valid() || !pPhysics->isBodyActive( m_physicsBodyHandle ) )
		return;

	Transformf t = pPhysics->getBodyTransform( m_physicsBodyHandle );
	m_transform.setPositionRotation( t.position, t.rotation );
}

///////////////////////////////////////////////////////////////////////////////////////
/*
wv::Rigidbody* wv::Rigidbody::parseInstance( ParseData& _data )
{
	return Runtime::create<Rigidbody>();
}
*/
