#include "Material.h"

#include <wv/Application/Application.h>
#include <wv/Assets/Texture.h>
#include <wv/Camera/ICamera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Scene/Model.h>

#include <glm/glm.hpp>
#include <wv/Auxiliary/fkYAML/node.hpp>

void wv::Material::load( const char* _path )
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	wv::MemoryDevice mdevice;
	std::string yaml = mdevice.loadString( _path );
	fkyaml::node root = fkyaml::node::deserialize( yaml );
	std::string shader = root[ "shader" ].get_value<std::string>();

	m_pipeline = mdevice.loadShaderPipeline( "res/shaders/" + shader + ".wshader" );

	for ( auto& textureFile : root[ "textures" ] )
	{
		std::string texturePath = "res/textures/" + textureFile[ 1 ].get_value<std::string>();
		TextureMemory texMem = mdevice.loadTextureData( texturePath.c_str() );
		TextureDesc texDesc; texDesc.memory = &texMem;
		//texDesc.generateMipMaps = true;
		
		Texture* tex = device->createTexture(&texDesc);
		if ( tex )
			m_textures.push_back( tex );
		
		mdevice.unloadTextureData( &texMem );
	}
}

void wv::Material::destroy()
{
	wv::GraphicsDevice* device = wv::Application::get()->device;

	/// TODO: move to some resource/texture manager
	for ( int i = 0; i < (int)m_textures.size(); i++ )
		device->destroyTexture( &m_textures[ i ] );
	m_textures.clear();

	/// TODO: unload pipeline?
}

void wv::Material::setAsActive( GraphicsDevice* _device )
{
	_device->setActivePipeline( m_pipeline );
	materialCallback();
}

void wv::Material::materialCallback()
{
	wv::Application* app = wv::Application::get();
	wv::Context* ctx = app->context;

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

void wv::Material::instanceCallback( Node* _instance )
{
	wv::Application* app = wv::Application::get();
	wv::Context* ctx = app->context;

	// model transform
	wv::UniformBlock& instanceBlock = m_pipeline->uniformBlocks[ "UbInstanceData" ];

	Model* mesh = reinterpret_cast<Model*>( _instance );
	instanceBlock.set( "u_Model", mesh->m_transform.getMatrix() );
}
