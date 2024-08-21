#include "Rigidbody.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Memory/ModelParser.h>
#include <wv/Memory/MemoryDevice.h>

#include <wv/Physics/PhysicsEngine.h>

#include <wv/Physics/PhysicsBodyDescriptor.h>

///////////////////////////////////////////////////////////////////////////////////////

cRigidbody::cRigidbody( const uint64_t& _uuid, const std::string& _name, wv::Mesh* _pMesh, wv::iPhysicsBodyDesc* _bodyDesc ) :
	iSceneObject{ _uuid, _name },
	m_pMesh{ _pMesh },
	m_pPhysicsBodyDesc{ _bodyDesc }
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

	if( !m_pMesh )
	{
		wv::assimp::Parser parser;
		m_pMesh = parser.load( "res/meshes/debug-cube.dae", app->m_pMaterialRegistry );
	}
	/// TODO: physics
	//sphereSettings.mLinearVelocity = JPH::Vec3( 1.0f, 10.0f, 2.0f );
	//sphereSettings.mRestitution = 0.4f;

	m_pPhysicsBodyDesc->transform = m_transform;
	m_physicsBodyHandle = app->m_pPhysicsEngine->createAndAddBody( m_pPhysicsBodyDesc, true );
}

///////////////////////////////////////////////////////////////////////////////////////

void cRigidbody::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_pMesh );
}

///////////////////////////////////////////////////////////////////////////////////////

void cRigidbody::updateImpl( double _deltaTime )
{
	if( m_physicsBodyHandle > 0 )
	{
		wv::Transformf t = wv::cEngine::get()->m_pPhysicsEngine->getPhysicsBodyTransform( m_physicsBodyHandle );
		m_transform.position = t.position;
		m_transform.rotation = t.rotation;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void cRigidbody::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	if( m_pMesh )
	{
		m_pMesh->transform = m_transform;
		_device->draw( m_pMesh );
	}
}
