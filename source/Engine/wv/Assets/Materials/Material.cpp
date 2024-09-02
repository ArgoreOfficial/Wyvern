#include "Material.h"

#include <wv/Engine/Engine.h>
#include <wv/Assets/Texture.h>
#include <wv/Camera/Camera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Math/Transform.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Primitive/Mesh.h>

#include <wv/Shader/ShaderRegistry.h>

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

void wv::cMaterial::load( cFileSystem* _pFileSystem )
{

	iResource::load( _pFileSystem );
}

void wv::cMaterial::unload( cFileSystem* _pFileSystem )
{

	iResource::unload( _pFileSystem );
}

void wv::cMaterial::create( iGraphicsDevice* _pGraphicsDevice )
{

	iResource::create( _pGraphicsDevice );
}

void wv::cMaterial::destroy( iGraphicsDevice* _pGraphicsDevice )
{


	iResource::destroy( _pGraphicsDevice );
}

void wv::cMaterial::setAsActive( iGraphicsDevice* _device )
{
	_device->useProgram( m_program );
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

	// camera transorm
	wv::UniformBlock& block = *m_program->getUniformBlock( "UbInstanceData" );

	cMatrix4x4f projection = app->currentCamera->getProjectionMatrix();
	cMatrix4x4f view       = app->currentCamera->getViewMatrix();
	cMatrix4x4f model{ 1.0f };

	block.set( "u_Projection", projection );
	block.set( "u_View", view );
	block.set( "u_Model", model );
}

void wv::cMaterial::setDefaultMeshUniforms( Mesh* _mesh )
{
	wv::cEngine* app = wv::cEngine::get();

	// model transform
	wv::UniformBlock& instanceBlock = *m_program->getUniformBlock( "UbInstanceData" );

	instanceBlock.set( "u_Model", _mesh->transform.getMatrix() );

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
}
