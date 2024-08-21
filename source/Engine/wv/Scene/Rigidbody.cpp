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

wv::cRigidbody::cRigidbody( const wv::UUID& _uuid, const std::string& _name, wv::Mesh* _pMesh, wv::iPhysicsBodyDesc* _bodyDesc ) :
	iSceneObject{ _uuid, _name },
	m_pMesh{ _pMesh },
	m_pPhysicsBodyDesc{ _bodyDesc }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody::~cRigidbody()
{

}

///////////////////////////////////////////////////////////////////////////////////////

nlohmann::json wv::cRigidbody::dataToJson( void )
{

	return nlohmann::json();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::onLoadImpl()
{
	cEngine* app = wv::cEngine::get();

	if( !m_pMesh )
	{
		assimp::Parser parser;
		m_pMesh = parser.load( "res/meshes/debug-cube.dae", app->m_pMaterialRegistry );
	}
	/// TODO: physics
	//sphereSettings.mLinearVelocity = JPH::Vec3( 1.0f, 10.0f, 2.0f );
	//sphereSettings.mRestitution = 0.4f;
#ifdef WV_SUPPORT_PHYSICS
	m_pPhysicsBodyDesc->transform = m_transform;
	m_physicsBodyHandle = app->m_pPhysicsEngine->createAndAddBody( m_pPhysicsBodyDesc, true );
#endif // WV_SUPPORT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_pMesh );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::updateImpl( double _deltaTime )
{
#ifdef WV_SUPPORT_PHYSICS
	if( m_physicsBodyHandle > 0 )
	{
		Transformf t = wv::cEngine::get()->m_pPhysicsEngine->getPhysicsBodyTransform( m_physicsBodyHandle );
		m_transform.position = t.position;
		m_transform.rotation = t.rotation;
	}
#endif // WV_SUPPORT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	if( m_pMesh )
	{
		m_pMesh->transform = m_transform;
		_device->draw( m_pMesh );
	}
}
