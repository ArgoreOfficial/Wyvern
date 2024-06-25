#include "Material.h"

#include <wv/Engine/Engine.h>
#include <wv/Assets/Texture.h>
#include <wv/Camera/Camera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Math/Transform.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Mesh.h>

#include <glm/glm.hpp>

#include <wv/Auxiliary/fkYAML/node.hpp>

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Material::loadFromFile( const char* _path )
{
	wv::cFileSystem mdevice;
	
	std::string path = std::string{ "res/materials/" } + _path + ".wmat";
	std::string yaml = mdevice.loadString( path );

	if ( yaml == "" )
		return false;

	return loadFromSource( yaml );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Material::loadFromSource( const std::string& _source )
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->device;
	wv::cFileSystem mdevice;

	fkyaml::node root = fkyaml::node::deserialize( _source );
	std::string shader = root[ "shader" ].get_value<std::string>();

	m_pipeline = Pipeline::loadFromFile( "res/shaders/" + shader + ".wshader" );

	if ( root[ "textures" ].is_sequence() )
	{
		for ( auto& textureFile : root[ "textures" ] )
		{
			std::string texturePath = "res/textures/" + textureFile[ 1 ].get_value<std::string>();
			TextureMemory* texMem = mdevice.loadTextureData( texturePath );
			TextureDesc texDesc;
			texDesc.memory = texMem;
			//texDesc.generateMipMaps = true;
			texDesc.filtering = WV_TEXTURE_FILTER_LINEAR;

			Texture* tex = device->createTexture( &texDesc );
			if ( tex )
				m_textures.push_back( tex );

			mdevice.unloadMemory( texMem );
		}
	}
	else
	{
		TextureMemory* texMem = mdevice.loadTextureData( "res/textures/uv.png" );
		TextureDesc texDesc;
		texDesc.memory = texMem;
		texDesc.filtering = WV_TEXTURE_FILTER_LINEAR;

		Texture* tex = device->createTexture( &texDesc );
		if ( tex )
			m_textures.push_back( tex );

		mdevice.unloadMemory( texMem );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::destroy()
{
	wv::iGraphicsDevice* device = wv::cEngine::get()->device;

	/// TODO: move to some resource/texture manager
	for ( int i = 0; i < (int)m_textures.size(); i++ )
		device->destroyTexture( &m_textures[ i ] );
	m_textures.clear();

	device->destroyPipeline( &m_pipeline );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::setAsActive( iGraphicsDevice* _device )
{
	_device->setActivePipeline( m_pipeline );
	materialCallback();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::materialCallback()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iDeviceContext* ctx = app->context;

	// camera transorm
	wv::UniformBlock& block = m_pipeline->uniformBlocks[ "UbInstanceData" ];

	glm::mat4x4 projection = app->currentCamera->getProjectionMatrix();
	glm::mat4x4 view = app->currentCamera->getViewMatrix();
	glm::mat4x4 model{ 1.0f };

	block.set( "u_Projection", projection );
	block.set( "u_View", view );
	block.set( "u_Model", model );

	// bind textures
	for ( int i = 0; i < (int)m_textures.size(); i++ )
		app->device->bindTextureToSlot( m_textures[ i ], i );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Material::instanceCallback( Mesh* _instance )
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iDeviceContext* ctx = app->context;

	// model transform
	wv::UniformBlock& instanceBlock = m_pipeline->uniformBlocks[ "UbInstanceData" ];

	instanceBlock.set( "u_Model", _instance->transform.getMatrix() );
}
