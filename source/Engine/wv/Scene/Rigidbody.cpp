#include "Rigidbody.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Memory/ModelParser.h>
#include <wv/Memory/MemoryDevice.h>

#include <wv/Physics/PhysicsEngine.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

///////////////////////////////////////////////////////////////////////////////////////

cRigidbody::cRigidbody( const uint64_t& _uuid, const std::string& _name, wv::Mesh* _mesh ) :
	iSceneObject{ _uuid, _name },
	m_mesh{_mesh}
{

}

///////////////////////////////////////////////////////////////////////////////////////

cRigidbody::~cRigidbody()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cRigidbody::onLoadImpl()
{
	wv::cEngine* app = wv::cEngine::get();

	if( !m_mesh )
	{
		wv::assimp::Parser parser;
		m_mesh = parser.load( "res/meshes/debug-cube.dae", app->m_pMaterialRegistry );
	}

	JPH::RVec3 joltPos{
		m_transform.position.x,
		m_transform.position.y,
		m_transform.position.z
	};

	JPH::RVec3 joltEuler{
		m_transform.rotation.x,
		m_transform.rotation.y,
		m_transform.rotation.z
	};

	JPH::BodyCreationSettings sphereSettings(
		// new JPH::SphereShape( 0.5f ),
		new JPH::BoxShape( JPH::RVec3{ 0.5f, 0.5f, 0.5f } ),
		joltPos,
		JPH::Quat::sEulerAngles( joltEuler ),
		JPH::EMotionType::Dynamic, wv::Layers::DYNAMIC
	);
	//sphereSettings.mLinearVelocity = JPH::Vec3( 1.0f, 10.0f, 2.0f );
	sphereSettings.mRestitution = 0.4f;

	m_physicsBodyHandle = app->m_pPhysicsEngine->createAndAddBody( sphereSettings, true );
}

///////////////////////////////////////////////////////////////////////////////////////

void cRigidbody::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_mesh );
}

///////////////////////////////////////////////////////////////////////////////////////

void cRigidbody::updateImpl( double _deltaTime )
{
	if( m_physicsBodyHandle > 0 )
		m_transform = wv::cEngine::get()->m_pPhysicsEngine->getPhysicsBodyTransform( m_physicsBodyHandle );
	
	printf( "%f\n", m_transform.position.y );
}

///////////////////////////////////////////////////////////////////////////////////////

void cRigidbody::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	if( m_mesh )
	{
		m_mesh->transform = m_transform;
		_device->draw( m_mesh );
	}
}
