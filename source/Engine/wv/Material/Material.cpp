#include "Material.h"

#include <wv/Texture/TextureResource.h>
#include <wv/Camera/Camera.h>
#include <wv/Graphics/Graphics.h>
#include <wv/Engine/Engine.h>
#include <wv/Math/Transform.h>
#include <wv/Mesh/MeshResource.h>
#include <wv/Graphics/Mesh.h>
#include <wv/Shader/ShaderResource.h>
#include <wv/Resource/ResourceRegistry.h>

#include <wv/Auxiliary/json/json11.hpp>
#include <wv/Memory/FileSystem.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::load( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
{
	if ( m_path == "" )
		m_path = _pFileSystem->getFullPath( m_name + ".wmat" );

	std::string jsonSource = _pFileSystem->loadString( m_path );

	std::string err;
	json11::Json root = json11::Json::parse( jsonSource, err );

	std::string shaderName = root[ "shader" ].string_value();

	cResourceRegistry* pResReg = cEngine::get()->m_pResourceRegistry;

	m_pShader = pResReg->load<cShaderResource>( shaderName );

	for ( auto& textureObject : root[ "textures" ].array_items() )
	{
		std::string uniformName = textureObject[ "name" ].string_value();
		std::string textureName = textureObject[ "texture" ].string_value();
		int filtering = 0;

		auto filteringObj = textureObject[ "filtering" ];
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

		cTextureResource* texture = pResReg->load<cTextureResource>( textureName, (wv::eTextureFiltering)filtering );
		textureVariable.data.texture = texture;

		m_variables.push_back( textureVariable );
	}

	setComplete( true );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::unload( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
{
	setComplete( false );

	cResourceRegistry* resReg = cEngine::get()->m_pResourceRegistry;
	
	if( m_pShader )
		resReg->unload( m_pShader );

	for( auto& var : m_variables )
	{
		switch( var.type )
		{
		case wv::WV_MATERIAL_VARIABLE_TEXTURE: resReg->unload( var.data.texture ); break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMaterial::setAsActive( iLowLevelGraphics* _device )
{
	m_pShader->bind( _device );
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
	if ( m_pShader == nullptr || !m_pShader->isComplete() )
		return;

	wv::cEngine* app = wv::cEngine::get();

	m_UbInstanceData.model = _mesh->transform.getMatrix();

#if defined( WV_PLATFORM_PSVITA )

#elif defined( WV_PLATFORM_WINDOWS )
	// model transform
	wv::GPUBufferID instanceBlockID = m_pShader->getShaderBuffer( "UbInstanceData" );
	wv::sGPUBuffer& instanceBlock = app->graphics->m_gpuBuffers.get( instanceBlockID );
	
	app->graphics->bufferSubData( instanceBlockID, &m_UbInstanceData, sizeof( sUbInstanceData ), 0 );
#endif
}

