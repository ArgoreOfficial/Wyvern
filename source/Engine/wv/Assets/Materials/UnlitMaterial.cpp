#include "UnlitMaterial.h"

#include <wv/Application/Application.h>
#include <wv/Assets/Texture.h>
#include <wv/Camera/iCamera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Scene/Mesh.h>

#include <glm/glm.hpp>

void wv::UnlitMaterial::create( GraphicsDevice* _device )
{	
	// create pipeline
	/// TODO: get from some pipeline manager
	{
		wv::ShaderSource shaders[] = {
				{ wv::WV_SHADER_TYPE_VERTEX,   "res/vert.glsl" },
				{ wv::WV_SHADER_TYPE_FRAGMENT, "res/frag.glsl" }
		};

		const char* ubInstanceDataUniforms[] = {
			"u_Projection",
			"u_View",
			"u_Model",
		};

		wv::UniformBlockDesc uniformBlocks[] = {
			{ "UbInstanceData", ubInstanceDataUniforms, 3 }
		};

		wv::PipelineDesc pipelineDesc;
		pipelineDesc.type = wv::WV_PIPELINE_GRAPHICS;
		pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
		pipelineDesc.shaders = shaders;
		pipelineDesc.uniformBlocks = uniformBlocks;
		pipelineDesc.numUniformBlocks = 1;
		pipelineDesc.numShaders = 2;
		
		m_pipeline = _device->createPipeline( &pipelineDesc );
	}

	// create albedo texture
	/// TODO: get from some texture manager
	{
		wv::Application* app = wv::Application::get();
		TextureMemory texMem = app->memoryDevice->loadTextureData( "res/throbber.gif" );
		TextureDesc texDesc;
		texDesc.memory = &texMem;
		m_albedoTexture = _device->createTexture( &texDesc );
		app->memoryDevice->unloadTextureData( &texMem );
	}
}

void wv::UnlitMaterial::destroy( GraphicsDevice* _device )
{
	_device->destroyPipeline( &m_pipeline );
	_device->destroyTexture( &m_albedoTexture );
}

void wv::UnlitMaterial::materialCallback()
{
	wv::Application* app = wv::Application::get();
	wv::Context* ctx = app->context;
	
	// camera transorm
	wv::UniformBlock& block = m_pipeline->uniformBlocks[ "UbInstanceData" ];

	glm::mat4x4 projection = app->currentCamera->getProjectionMatrix();
	glm::mat4x4 view       = app->currentCamera->getViewMatrix();

	block.set( "u_Projection", projection );
	block.set( "u_View", view );

	// bind texture to slot 0
	app->device->bindTextureToSlot( m_albedoTexture, 0 );
}

void wv::UnlitMaterial::instanceCallback( Node* _instance )
{
	wv::Application* app = wv::Application::get();
	wv::Context* ctx = app->context;

	// model transform
	wv::UniformBlock& instanceBlock = m_pipeline->uniformBlocks[ "UbInstanceData" ];

	Mesh* mesh = reinterpret_cast<Mesh*>( _instance );
	instanceBlock.set( "u_Model", mesh->m_transform.getMatrix() );
}
