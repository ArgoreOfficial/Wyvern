#include "Rigidbody.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/GraphicsDevice.h>

#include <wv/Mesh/MeshResource.h>
#include <wv/Material/Material.h>

#include <wv/Memory/ModelParser.h>
#include <wv/Memory/FileSystem.h>

#include <wv/Physics/PhysicsEngine.h>
#include <wv/Physics/PhysicsBodyDescriptor.h>

#include <wv/Resource/ResourceRegistry.h>

#include <wv/Scene/Component/ModelComponent.h>
#include <wv/Scene/Component/RigidBodyComponent.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody::cRigidbody( const UUID& _uuid, const std::string& _name ) :
	Entity{ _uuid, _name }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cRigidbody::cRigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, iPhysicsBodyDesc* _bodyDesc ) : 
	Entity{ _uuid, _name },
	m_meshPath{ _meshPath },
	m_bodyDesc{ _bodyDesc }
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
	Vector3f pos = jsonToVec3( tfm[ "pos" ].array_items() );
	Vector3f rot = jsonToVec3( tfm[ "rot" ].array_items() );
	Vector3f scl = jsonToVec3( tfm[ "scl" ].array_items() );

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
	case WV_PHYSICS_NONE: break;

	case WV_PHYSICS_SPHERE:
	{
		sPhysicsSphereDesc* sphereDesc = WV_NEW( sPhysicsSphereDesc );
		sphereDesc->radius = data[ "radius" ].number_value();
		desc = sphereDesc;
	} break;

	case WV_PHYSICS_BOX:
	{
		wv::Json::array halfExtents = data[ "halfExtents" ].array_items();

		sPhysicsBoxDesc* boxDesc = WV_NEW( sPhysicsBoxDesc );
		boxDesc->halfExtent.x = halfExtents[ 0 ].number_value();
		boxDesc->halfExtent.y = halfExtents[ 1 ].number_value();
		boxDesc->halfExtent.z = halfExtents[ 2 ].number_value();
		desc = boxDesc;
	} break;

	case WV_PHYSICS_CAPSULE:          break;
	case WV_PHYSICS_TAPERRED_CAPSULE: break;
	case WV_PHYSICS_CYLINDER:         break;
	case WV_PHYSICS_CONVECT_HULL:     break;
	case WV_PHYSICS_PLANE:            break;
	case WV_PHYSICS_MESH:             break;
	case WV_PHYSICS_TERRAIN:          break;

	}

	if( desc )
		desc->kind = kind;

	std::string meshPath = data[ "path" ].string_value();

	cRigidbody* rb = WV_NEW( cRigidbody, uuid, name, meshPath, desc );
	rb->m_transform = transform;
	rb->addComponent<ModelComponent>( meshPath );
	rb->addComponent<RigidBodyComponent>( desc );

	return rb;
}

///////////////////////////////////////////////////////////////////////////////////////
