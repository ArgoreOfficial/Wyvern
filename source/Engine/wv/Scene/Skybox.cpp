#include "Skybox.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/GraphicsDevice.h>

#include <wv/Mesh/MeshResource.h>
#include <wv/Material/Material.h>
#include <wv/Resource/ResourceRegistry.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <fstream>


///////////////////////////////////////////////////////////////////////////////////////

wv::cSkyboxObject::cSkyboxObject( const UUID& _uuid, const std::string& _name ) :
	IEntity{ _uuid, _name }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cSkyboxObject::~cSkyboxObject()
{
	
}

wv::cSkyboxObject* wv::cSkyboxObject::parseInstance( sParseData& _data )
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

	cSkyboxObject* skysphere = WV_NEW( cSkyboxObject, uuid, name );
	skysphere->m_transform = transform;

	return skysphere;
}

void wv::cSkyboxObject::onConstructImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	
	m_mesh = app->m_pResourceRegistry->load<cMeshResource>( "meshes/skysphere.glb" )->createInstance();
	m_transform.addChild( &m_mesh.transform );
}

void wv::cSkyboxObject::onDeconstructImpl()
{
	m_mesh.destroy();
}

void wv::cSkyboxObject::onEnterImpl()
{

}

void wv::cSkyboxObject::onExitImpl()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cSkyboxObject::onUpdate( double /*_deltaTime*/ )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cSkyboxObject::onDraw( iDeviceContext* /*_context*/, IGraphicsDevice* /*_device*/ )
{
	
}
