#include "Skybox.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Mesh/MeshResource.h>
#include <wv/Material/Material.h>
#include <wv/Resource/ResourceRegistry.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <fstream>


///////////////////////////////////////////////////////////////////////////////////////

wv::cSkyboxObject::cSkyboxObject( const UUID& _uuid, const std::string& _name ) :
	iSceneObject{ _uuid, _name }
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
	cVector3f pos = jsonToVec3( tfm[ "pos" ].array_items() );
	cVector3f rot = jsonToVec3( tfm[ "rot" ].array_items() );
	cVector3f scl = jsonToVec3( tfm[ "scl" ].array_items() );

	Transformf transform;
	transform.setPosition( pos );
	transform.setRotation( rot );
	transform.setScale( scl );

	cSkyboxObject* skysphere = new cSkyboxObject( uuid, name );
	skysphere->m_transform = transform;

	return skysphere;
}

void wv::cSkyboxObject::onLoadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	
	m_mesh = app->m_pResourceRegistry->load<cMeshResource>( "res/meshes/skysphere.dae" )->createInstance();
	m_transform.addChild( &m_mesh.transform );
}

void wv::cSkyboxObject::onUnloadImpl()
{
	m_mesh.destroy();
}

void wv::cSkyboxObject::onCreateImpl()
{

}

void wv::cSkyboxObject::onDestroyImpl()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cSkyboxObject::updateImpl( double _deltaTime )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cSkyboxObject::drawImpl( iDeviceContext* _context, iGraphicsDevice* _device )
{
	/// TODO: remove raw gl calls
#ifdef WV_SUPPORT_OPENGL
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	m_mesh.draw();
	m_mesh.pResource->drawInstances( _device ); 
	glDepthFunc( GL_LESS );
	glDepthMask( GL_TRUE );
#endif
	
}
