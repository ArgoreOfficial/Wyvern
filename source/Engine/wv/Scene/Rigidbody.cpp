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
	m_meshPath{ "" },
	m_pPhysicsBodyDesc{ _bodyDesc }
{

}

wv::cRigidbody::cRigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, iPhysicsBodyDesc* _bodyDesc ) :
	iSceneObject{ _uuid, _name },
	m_pMesh{ nullptr },
	m_meshPath{ _meshPath },
	m_pPhysicsBodyDesc{ _bodyDesc }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody::~cRigidbody()
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody* wv::cRigidbody::createInstance()
{

	return nullptr;
}

wv::cRigidbody* wv::cRigidbody::createInstanceJson( nlohmann::json& _json )
{
	wv::UUID    uuid = _json.value( "uuid", cEngine::getUniqueUUID() );
	std::string name = _json.value( "name", "" );

	nlohmann::json tfm = _json[ "transform" ];
	std::vector<float> pos = tfm[ "pos" ].get<std::vector<float>>();
	std::vector<float> rot = tfm[ "rot" ].get<std::vector<float>>();
	std::vector<float> scl = tfm[ "scl" ].get<std::vector<float>>();

	Transformf transform;
	transform.setPosition( { pos[ 0 ], pos[ 1 ], pos[ 2 ] } );
	transform.setRotation( { rot[ 0 ], rot[ 1 ], rot[ 2 ] } );
	transform.setScale( { scl[ 0 ], scl[ 1 ], scl[ 2 ] } );

	nlohmann::json data = _json[ "data" ];

	ePhysicsKind  kind = data.value( "kind", ePhysicsKind::WV_PHYSICS_STATIC );
	ePhysicsShape shape = data.value( "shape", ePhysicsShape::WV_PHYSICS_BOX );

	iPhysicsBodyDesc* desc = nullptr;
	
	switch( shape )
	{
	case WV_PHYSICS_BOX:
	{
		std::vector<float> halfExtents = data[ "halfExtents" ].get<std::vector<float>>();

		sPhysicsBoxDesc* boxDesc = new sPhysicsBoxDesc();
		boxDesc->halfExtent.x = halfExtents[ 0 ];
		boxDesc->halfExtent.y = halfExtents[ 1 ];
		boxDesc->halfExtent.z = halfExtents[ 2 ];
		desc = boxDesc;
	} break;

	case WV_PHYSICS_SPHERE:
	{
		sPhysicsSphereDesc* sphereDesc = new sPhysicsSphereDesc();
		sphereDesc->radius = data.value( "radius", 1.0f );
		desc = sphereDesc;
	} break;
	}

	if( desc )
		desc->kind = kind;

	std::string meshPath = data.value( "path", "" );

	cRigidbody* rb = new cRigidbody( uuid, name, meshPath, desc );
	rb->m_transform = transform;
	return rb;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::onLoadImpl()
{
	cEngine* app = wv::cEngine::get();

	if( m_pMesh == nullptr )
	{
		if( m_meshPath == "" )
		{
			Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
			m_meshPath = "res/meshes/debug-cube.dae";
		}

		wv::assimp::Parser parser;
		m_pMesh = parser.load( m_meshPath.c_str(), app->m_pMaterialRegistry );
	}

	//sphereSettings.mLinearVelocity = JPH::Vec3( 1.0f, 10.0f, 2.0f );
	//sphereSettings.mRestitution = 0.4f;
#ifdef WV_SUPPORT_PHYSICS
	m_pPhysicsBodyDesc->transform = m_transform;
	m_physicsBodyHandle = app->m_pPhysicsEngine->createAndAddBody( m_pPhysicsBodyDesc, true );
#endif // WV_SUPPORT_PHYSICS
	delete m_pPhysicsBodyDesc;
	m_pPhysicsBodyDesc = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	app->m_pPhysicsEngine->destroyPhysicsBody( m_physicsBodyHandle );
	
	if( m_pMesh )
		device->destroyMesh( &m_pMesh );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::updateImpl( double _deltaTime )
{
#ifdef WV_SUPPORT_PHYSICS
	if( m_physicsBodyHandle.isValid() )
	{
		Transformf t = wv::cEngine::get()->m_pPhysicsEngine->getBodyTransform( m_physicsBodyHandle );
		m_transform.position = t.position;
		m_transform.rotation = t.rotation;
		m_transform.parent = nullptr;
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
