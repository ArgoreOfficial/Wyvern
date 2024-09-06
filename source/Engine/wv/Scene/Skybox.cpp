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

	return new cSkyboxObject( uuid, name );
}

void wv::cSkyboxObject::onLoadImpl()
{
	wv::cEngine* app = wv::cEngine::get();

	wv::assimp::Parser parser;
	m_skyboxMesh = parser.load( "res/meshes/skysphere.dae", app->m_pMaterialRegistry );
	m_skyboxMesh->transform.rotation.x = -90.0f;
	m_skyboxMesh->transform.update();

	m_skyMaterial = app->m_pMaterialRegistry->loadMaterial( "sky" );

	// resource leak
	if ( m_skyboxMesh )
		m_skyboxMesh->primitives[ 0 ]->material = m_skyMaterial;
}

void wv::cSkyboxObject::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_skyboxMesh );
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
	if ( m_skyboxMesh && m_skyMaterial->isCreated() && m_skyMaterial->getProgram()->isCreated() )
	{
		/// TODO: remove raw gl calls
	#ifdef WV_SUPPORT_OPENGL
		glDepthMask( GL_FALSE );
		glDepthFunc( GL_LEQUAL );
		_device->draw( m_skyboxMesh );
		glDepthFunc( GL_LESS );
		glDepthMask( GL_TRUE );
	#endif
	}
}
