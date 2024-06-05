#include "UnlitMaterial.h"

#include <wv/Application/Application.h>
#include <wv/Assets/Texture.h>
#include <wv/Camera/iCamera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Scene/Model.h>

#include <glm/glm.hpp>

void wv::UnlitMaterial::create( GraphicsDevice* _device, const char* _vs, const char* _fs )
{	
	// create pipeline
	/// TODO: get from some pipeline manager
	{
		wv::ShaderSource shaders[] = {
				{ wv::WV_SHADER_TYPE_VERTEX,   _vs ? _vs : "res/shaders/unlit_vs.glsl" },
				{ wv::WV_SHADER_TYPE_FRAGMENT, _fs ? _fs : "res/shaders/unlit_fs.glsl" }
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
		pipelineDesc.name = "Unlit";
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
		TextureMemory texMem = app->memoryDevice->loadTextureData( "res/textures/skybox-mountain.png" );
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
	glm::mat4x4 model{1.0f};

	block.set( "u_Projection", projection );
	block.set( "u_View", view );
	block.set( "u_Model", model );

	// bind texture to slot 0
	app->device->bindTextureToSlot( m_albedoTexture, 0 );
}

void wv::UnlitMaterial::instanceCallback( Node* _instance )
{
	wv::Application* app = wv::Application::get();
	wv::Context* ctx = app->context;

	// model transform
	wv::UniformBlock& instanceBlock = m_pipeline->uniformBlocks[ "UbInstanceData" ];

	Model* mesh = reinterpret_cast<Model*>( _instance );
	instanceBlock.set( "u_Model", mesh->m_transform.getMatrix() );
}
