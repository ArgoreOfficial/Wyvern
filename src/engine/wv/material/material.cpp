#include "material.h"

#include <wv/app_state.h>

#include <wv/texture/texture_resource.h>
#include <wv/camera/camera.h>
#include <wv/graphics/graphics_device.h>

#include <wv/engine.h>

#include <wv/math/transform.h>
#include <wv/mesh/mesh_resource.h>
#include <wv/graphics/mesh.h>
#include <wv/shader/shader_resource.h>
#include <wv/resource/resource_registry.h>

#include <auxiliary/json/json11.hpp>
#include <wv/filesystem/file_system.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::load( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice )
{
	if ( m_path == "" )
		m_path = m_name + ".wmat";

	std::string jsonSource = _pFileSystem->loadString( m_path );

	std::string err;
	json11::Json root = json11::Json::parse( jsonSource, err );

	std::string shaderName = root[ "shader" ].string_value();

	ResourceRegistry* pResReg = Engine::get()->m_pResourceRegistry;

	m_pShader = pResReg->load<ShaderResource>( shaderName );

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

		MaterialVariable textureVariable;
		textureVariable.handle = 0;
		textureVariable.name = uniformName;
		textureVariable.type = WV_MATERIAL_VARIABLE_TEXTURE;

		TextureResource* texture = pResReg->load<TextureResource>( textureName, (wv::TextureFiltering)filtering );
		textureVariable.data.texture = texture;

		m_variables.push_back( textureVariable );
	}

	setComplete( true );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::unload( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice )
{
	setComplete( false );

	ResourceRegistry* resReg = Engine::get()->m_pResourceRegistry;
	
	if( m_pShader )
		resReg->unload( m_pShader );

	for( auto& var : m_variables )
	{
		switch( var.type )
		{
		case wv::WV_MATERIAL_VARIABLE_TEXTURE: resReg->unload( var.data.texture ); break;
		case wv::WV_MATERIAL_VARIABLE_INT:   break;
		case wv::WV_MATERIAL_VARIABLE_FLOAT: break;
		case wv::WV_MATERIAL_VARIABLE_RGBA:  break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::setAsActive( IGraphicsDevice* _device )
{
	m_pShader->bind( _device );
	setMaterialUniforms();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::setMaterialUniforms()
{
	if ( m_pShader == nullptr || !m_pShader->isComplete() )
		return;

	wv::Engine* app = wv::Engine::get();

	// model transform
	wv::GPUBufferID instanceBlockID = m_pShader->getShaderBuffer( "UbCameraData" );
	wv::GPUBuffer& instanceBlock = app->graphics->m_gpuBuffers.at( instanceBlockID );

	GPUBufferID id = GetAppState()->currentCamera->getBufferID();

	app->graphics->bindBufferIndex( id, instanceBlock.bindingIndex.value );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::setInstanceUniforms( Mesh* _instance )
{
	setDefaultMeshUniforms( _instance ); // sets transform/model matrix
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::setDefaultMeshUniforms( Mesh* _mesh )
{
	
}

