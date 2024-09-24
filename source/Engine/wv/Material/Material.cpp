#include "Material.h"

#include <wv/Texture/Texture.h>
#include <wv/Camera/Camera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Engine/Engine.h>
#include <wv/Math/Transform.h>
#include <wv/Mesh/MeshResource.h>
#include <wv/Mesh/Mesh.h>

#include <wv/Auxiliary/json/json11.hpp>
#include <wv/Memory/FileSystem.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::load( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{
	if ( m_path == "" )
		m_path = _pFileSystem->getFullPath( m_name + ".wmat" );

	std::string jsonSource = _pFileSystem->loadString( m_path );

	std::string err;
	json11::Json root = json11::Json::parse( jsonSource, err );

	std::string shaderName = root[ "shader" ].string_value();

	m_pPipeline = new cProgramPipeline( shaderName );
	m_pPipeline->load( _pFileSystem, _pGraphicsDevice );

	while ( !m_pPipeline->isComplete() )
	{
		/// TEMPORARY FIX
		/// TODO: NOT THIS
	#ifdef WV_PLATFORM_WINDOWS 
		Sleep( 1 );
		//_pGraphicsDevice->endRender();
	#endif
	}

#ifdef WV_PLATFORM_WINDOWS
	for ( auto& textureObject : root[ "textures" ].array_items() )
	{
		std::string uniformName = textureObject[ "name" ].string_value();
		std::string textureName = textureObject[ "texture" ].string_value();
		auto filteringObj = textureObject[ "filtering" ];
		int filtering = 0;

		if ( !textureObject.is_null() )
			filtering = filteringObj.int_value();

		if ( uniformName == "" || textureName == "" )
		{
			Debug::Print( Debug::WV_PRINT_ERROR, "Texture uniform is empty\n" );
			continue;
		}

		sMaterialVariable textureVariable;
		textureVariable.handle = 0;
		textureVariable.name = uniformName;
		textureVariable.type = WV_MATERIAL_VARIABLE_TEXTURE;

		cTextureResource* texture = new cTextureResource( textureName, "", (wv::eTextureFiltering)filtering );
		texture->load( _pFileSystem, _pGraphicsDevice );
		textureVariable.data.texture = texture;

		//m_resourceLoader.addLoad( texture );

		while ( !texture->isComplete() )
		{
		#ifdef WV_PLATFORM_WINDOWS 
			Sleep( 1 );
			//_pGraphicsDevice->endRender();
		#endif
		}

		m_variables.push_back( textureVariable );
	}

#endif

	setComplete( true );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{
	setComplete( false );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::setAsActive( iGraphicsDevice* _device )
{
	m_pPipeline->use( _device );
	setMaterialUniforms();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::setMaterialUniforms()
{
	setDefaultViewUniforms(); // sets projection and view matrices
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::setInstanceUniforms( sMesh* _instance )
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

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::setDefaultMeshUniforms( sMesh* _mesh )
{
	wv::cEngine* app = wv::cEngine::get();

	m_UbInstanceData.model = _mesh->transform.getMatrix();

#if defined( WV_PLATFORM_PSVITA )

#elif defined( WV_PLATFORM_WINDOWS )
	// model transform
	wv::cGPUBuffer& instanceBlock = *m_pPipeline->getShaderBuffer( "UbInstanceData" );
	instanceBlock.buffer( &m_UbInstanceData );
	
	// bind textures
	int texSlot = 0;
	for( int i = 0; i < ( int )m_variables.size(); i++ )
	{
		if( m_variables[ i ].type != WV_MATERIAL_VARIABLE_TEXTURE )
			continue;

		/// TODO: move and change to instance variables
		app->graphics->bindTextureToSlot( &m_variables[ i ].data.texture->m_texture, texSlot );
		texSlot++;
	}
#endif
}

