#include "Rigidbody.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Memory/ModelParser.h>
#include <wv/Memory/FileSystem.h>

#include <wv/Physics/PhysicsEngine.h>
#include <wv/Physics/PhysicsBodyDescriptor.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody::cRigidbody( const wv::UUID& _uuid, const std::string& _name, wv::sMeshNode* _pMeshNode, wv::iPhysicsBodyDesc* _bodyDesc ) :
	iSceneObject{ _uuid, _name },
	m_pMeshNode{ _pMeshNode },
	m_meshPath{ "" },
	m_pPhysicsBodyDesc{ _bodyDesc }
{

}

wv::cRigidbody::cRigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, iPhysicsBodyDesc* _bodyDesc ) :
	iSceneObject{ _uuid, _name },
	m_pMeshNode{ nullptr },
	m_meshPath{ _meshPath },
	m_pPhysicsBodyDesc{ _bodyDesc }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody::~cRigidbody()
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody* wv::cRigidbody::parseInstance( sParseData& _data )
{
	wv::Json& json = _data.json;
	std::string name = json[ "name" ].string_value();
	wv::UUID    uuid = json[ "uuid" ].int_value();

	wv::Json tfm = json[ "transform" ];
	cVector3f pos = jsonToVec3( tfm[ "pos" ].array_items() );
	cVector3f rot = jsonToVec3( tfm[ "rot" ].array_items() );
	cVector3f scl = jsonToVec3( tfm[ "scl" ].array_items() );

	Transformf transform;
	transform.setPosition( pos );
	transform.setRotation( rot );
	transform.setScale   ( scl );

	wv::Json data = json[ "data" ];

	ePhysicsKind  kind  = ( ePhysicsKind  )data[ "kind"  ].int_value();
	ePhysicsShape shape = ( ePhysicsShape )data[ "shape" ].int_value();

	iPhysicsBodyDesc* desc = nullptr;
	
	switch( shape )
	{
	case WV_PHYSICS_BOX:
	{
		wv::Json::array halfExtents = data[ "halfExtents" ].array_items();

		sPhysicsBoxDesc* boxDesc = new sPhysicsBoxDesc();
		boxDesc->halfExtent.x = halfExtents[ 0 ].number_value();
		boxDesc->halfExtent.y = halfExtents[ 1 ].number_value();
		boxDesc->halfExtent.z = halfExtents[ 2 ].number_value();
		desc = boxDesc;
	} break;

	case WV_PHYSICS_SPHERE:
	{
		sPhysicsSphereDesc* sphereDesc = new sPhysicsSphereDesc();
		sphereDesc->radius = data[ "radius" ].number_value();
		desc = sphereDesc;
	} break;
	}

	if( desc )
		desc->kind = kind;

	std::string meshPath = data[ "path" ].string_value();

	cRigidbody* rb = new cRigidbody( uuid, name, meshPath, desc );
	rb->m_transform = transform;
	return rb;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::onLoadImpl()
{
	cEngine* app = wv::cEngine::get();

	if( m_pMeshNode == nullptr )
	{
		if( m_meshPath == "" )
		{
			Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
			m_meshPath = "res/meshes/cube";
		}

		wv::Parser parser;
		m_pMeshNode = parser.load( m_meshPath.c_str(), app->m_pMaterialRegistry );
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

static void destroyMeshNode( wv::iGraphicsDevice* _graphics, wv::sMeshNode* _node )
{
	if( !_node )
		return;

	for( auto& mesh : _node->meshes )
		_graphics->destroyMesh( &mesh );

	for( auto& node : _node->children )
		destroyMeshNode( _graphics, node );
}

void wv::cRigidbody::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	app->m_pPhysicsEngine->destroyPhysicsBody( m_physicsBodyHandle );
	
	destroyMeshNode( device, m_pMeshNode );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::updateImpl( double _deltaTime )
{
#ifdef WV_SUPPORT_PHYSICS

	wv::cJoltPhysicsEngine* pPhysics = wv::cEngine::get()->m_pPhysicsEngine;

	if( m_physicsBodyHandle.isValid() && pPhysics->isBodyActive( m_physicsBodyHandle ) )
	{
		Transformf t = pPhysics->getBodyTransform( m_physicsBodyHandle );
		m_transform.position = t.position;
		m_transform.rotation = t.rotation;
		m_transform.parent = nullptr;
	}
#endif // WV_SUPPORT_PHYSICS
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cRigidbody::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	if( m_pMeshNode )
	{
		m_pMeshNode->transform = m_transform;
		_device->drawNode( m_pMeshNode );
	}
}
