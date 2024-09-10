#include "Material.h"

#include <wv/Assets/Texture.h>
#include <wv/Camera/Camera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Engine/Engine.h>
#include <wv/Math/Transform.h>
#include <wv/Primitive/Mesh.h>


///////////////////////////////////////////////////////////////////////////////////////

/*
void wv::iMaterial::destroy( iGraphicsDevice* _pGraphicsDevice )
{
	cEngine* app = cEngine::get();
	iGraphicsDevice* device = cEngine::get()->graphics;

	///// TODO: move to some resource/texture manager
	//for ( int i = 0; i < (int)m_textures.size(); i++ )
	//	device->destroyTexture( &m_textures[ i ] );
	//m_textures.clear();
	//
	//app->m_pShaderRegistry->unloadShaderProgram( m_program );

	iMaterial::destroy( _pGraphicsDevice );
}
*/

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::setAsActive( iGraphicsDevice* _device )
{
	m_program->use( _device );
	setMaterialUniforms();
}

void wv::cMaterial::setMaterialUniforms()
{
	setDefaultViewUniforms(); // sets projection and view matrices
}

void wv::cMaterial::setInstanceUniforms( Mesh* _instance )
{
	setDefaultMeshUniforms( _instance ); // sets transform/model matrix
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::setDefaultViewUniforms()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iDeviceContext* ctx = app->context;
	
	m_UbInstanceData.projection = wv::cMatrix4x4f( 1.0f );
	m_UbInstanceData.view       = wv::cMatrix4x4f( 1.0f );
	m_UbInstanceData.model      = wv::cMatrix4x4f( 1.0f );

	m_UbInstanceData.projection = app->currentCamera->getProjectionMatrix();
	m_UbInstanceData.view = app->currentCamera->getViewMatrix();
}

void wv::cMaterial::setDefaultMeshUniforms( Mesh* _mesh )
{
	wv::cEngine* app = wv::cEngine::get();

	m_UbInstanceData.model = _mesh->transform.getMatrix();

#if defined( WV_PLATFORM_PSVITA )

#elif defined( WV_PLATFORM_WINDOWS )
	// model transform
	wv::sGPUBuffer& instanceBlock = *m_program->getShaderBuffer( "UbInstanceData" );
	instanceBlock.buffer( &m_UbInstanceData );
	
	// bind textures
	int texSlot = 0;
	for( int i = 0; i < ( int )m_variables.size(); i++ )
	{
		if( m_variables[ i ].type != WV_MATERIAL_VARIABLE_TEXTURE )
			continue;

		/// TODO: move and change to instance variables
		app->graphics->bindTextureToSlot( m_variables[ i ].data.texture, texSlot );
		texSlot++;
	}
#endif
}
