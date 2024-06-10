#include "Pipeline.h"

#include <wv/Application/Application.h>
#include <wv/Auxiliary/fkYAML/node.hpp>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Shader/Shader.h>

wv::Pipeline* wv::Pipeline::loadFromFile( const std::string& _path )
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	MemoryDevice md;

	std::string yaml = md.loadString( _path.c_str() );
	fkyaml::node root = fkyaml::node::deserialize( yaml );

	std::string source = root[ "source" ].get_value<std::string>();
	wv::Pipeline* pipeline = device->getPipeline( source.c_str() );
	if ( pipeline )
		return pipeline;

	std::vector<wv::ShaderSource> shaders = {
		{ wv::WV_SHADER_TYPE_VERTEX,   "res/shaders/" + source + "_vs.glsl" },
		{ wv::WV_SHADER_TYPE_FRAGMENT, "res/shaders/" + source + "_fs.glsl" }
	};

	std::vector<UniformBlockDesc> blocks;
	int blockCounter = 0;
	for ( auto& block : root[ "blocks" ] )
	{
		UniformBlockDesc blockDesc{};

		blockDesc.name = block[ "block" ].get_value<std::string>();

		for ( auto& uniform : block[ "uniforms" ] )
		{
			Uniform u{ (unsigned int)blockCounter, 0, uniform[ "uniform" ].get_value<std::string>() };
			blockDesc.uniforms.push_back( u );
		}

		blockCounter++;
		blocks.push_back( blockDesc );
	}

	std::vector<Uniform> textureUniforms;;
	unsigned int textureCounter = 0;
	for ( auto& texture : root[ "textures" ] )
	{
		wv::Uniform u{ textureCounter, 0, texture.get_value<std::string>() };
		textureUniforms.push_back( u );
		textureCounter++;
	}

	wv::PipelineDesc pipelineDesc;
	pipelineDesc.name = source;
	pipelineDesc.type = wv::WV_PIPELINE_GRAPHICS;
	pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
	pipelineDesc.shaders = shaders;
	pipelineDesc.uniformBlocks = blocks;
	pipelineDesc.textureUniforms = textureUniforms;

	pipeline = device->createPipeline( &pipelineDesc );

	return pipeline;
}
