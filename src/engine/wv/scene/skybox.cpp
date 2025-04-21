#include "skybox.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/material/material.h>
#include <wv/resource/resource_registry.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <fstream>


///////////////////////////////////////////////////////////////////////////////////////

wv::SkyboxObject::SkyboxObject( const UUID& _uuid, const std::string& _name ) :
	Entity{ _uuid, _name }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::SkyboxObject::~SkyboxObject()
{
	
}
/*
wv::SkyboxObject* wv::SkyboxObject::parseInstance( ParseData& _data )
{	
	auto& json = _data.json;
	wv::UUID    uuid = json[ "uuid" ].int_value();
	std::string name = json[ "name" ].string_value();

	wv::Json tfm = json[ "transform" ];
	Vector3f pos = jsonToVec3( tfm[ "pos" ].array_items() );
	Vector3f rot = jsonToVec3( tfm[ "rot" ].array_items() );
	Vector3f scl = jsonToVec3( tfm[ "scl" ].array_items() );

	Transformf transform;
	transform.setPosition( pos );
	transform.setRotation( rot );
	transform.setScale( scl );

	SkyboxObject* skysphere = WV_NEW( SkyboxObject, uuid, name );
	skysphere->m_transform = transform;

	return skysphere;
}
*/

void wv::SkyboxObject::onConstruct()
{
	wv::Engine* app = wv::Engine::get();
	
	MeshResource* skysphere = app->m_pResourceRegistry->load<MeshResource>( "meshes/skysphere.glb" );
	skysphere->makeInstance( &m_mesh );

	m_transform.addChild( &m_mesh.transform );
}

void wv::SkyboxObject::onDestruct()
{
	m_mesh.destroy();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SkyboxObject::onUpdate( double /*_deltaTime*/ )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SkyboxObject::onDraw( IDeviceContext* /*_context*/, IGraphicsDevice* /*_device*/ )
{
	
}
