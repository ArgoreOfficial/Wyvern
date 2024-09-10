#include "Skybox.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Materials/MaterialRegistry.h>
#include <wv/Memory/ModelParser.h>

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
	wv::Parser parser;

	m_skyboxMesh = parser.load( "res/meshes/skysphere", app->m_pMaterialRegistry );
	m_transform.addChild( &m_skyboxMesh->transform );

	m_skyMaterial = app->m_pMaterialRegistry->loadMaterial( "sky" );
	
#ifdef WV_SUPPORT_OPENGL // temporary
	// bad
	if ( m_skyboxMesh )
		m_skyboxMesh->children[ 0 ]->meshes[ 0 ]->primitives[ 0 ]->material = m_skyMaterial;
#endif
}

void wv::cSkyboxObject::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_skyboxMesh->children[ 0 ]->meshes[ 0 ] );
	m_skyMaterial->decrNumUsers();

	// m_skyMaterial->destroy( device );
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
	if ( m_skyMaterial && m_skyboxMesh && m_skyMaterial->isComplete() && m_skyMaterial->getPipeline()->isComplete() )
	{
		/// TODO: remove raw gl calls
	#ifdef WV_SUPPORT_OPENGL
		glDepthMask( GL_FALSE );
		glDepthFunc( GL_LEQUAL );
		_device->drawNode( m_skyboxMesh );
		glDepthFunc( GL_LESS );
		glDepthMask( GL_TRUE );
	#endif
	}
}
