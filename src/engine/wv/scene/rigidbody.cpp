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
	m_bodyDesc{ _bodyDesc }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::Rigidbody::~Rigidbody()
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::Rigidbody* wv::Rigidbody::parseInstance( ParseData& _data )
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

	IPhysicsBodyDesc* desc = nullptr;
	
#ifdef WV_SUPPORT_PHYSICS
	PhysicsKind  kind  = ( PhysicsKind  )data[ "kind"  ].int_value();
	PhysicsShape shape = ( PhysicsShape )data[ "shape" ].int_value();

	switch( shape )
	{
	case WV_PHYSICS_NONE: break;

	case WV_PHYSICS_SPHERE:
	{
		PhysicsSphereDesc* sphereDesc = WV_NEW( PhysicsSphereDesc );
		sphereDesc->radius = data[ "radius" ].number_value();
		desc = sphereDesc;
	} break;

	case WV_PHYSICS_BOX:
	{
		wv::Json::array halfExtents = data[ "halfExtents" ].array_items();

		PhysicsBoxDesc* boxDesc = WV_NEW( PhysicsBoxDesc );
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
#endif

	std::string meshPath = data[ "path" ].string_value();
	Rigidbody* rb = WV_NEW( Rigidbody, uuid, name, meshPath, desc );
	rb->m_transform = transform;
	rb->addComponent<ModelComponent>( meshPath );
	rb->addComponent<RigidBodyComponent>( desc );

	return rb;
}

///////////////////////////////////////////////////////////////////////////////////////
