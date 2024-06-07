#include "MemoryDevice.h"

#include <wv/Application/Application.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Shader/UniformBlock.h>
#include <wv/Pipeline/Pipeline.h>

#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>

#include <wv/Auxiliary/json.hpp>
#include <wv/Auxiliary/fkYAML/node.hpp>

#include <fstream>
#include <vector>

wv::MemoryDevice::~MemoryDevice()
{
	if ( m_numLoadedFiles > 0 )
		printf( "Non-Empty MemoryDevice destroyed. This may cause memory leaks\n" );
}

wv::Memory wv::MemoryDevice::loadFromFile( const char* _path )
{
	std::ifstream in( _path, std::ios::binary );
	if ( !in.is_open() )
	{
		printf( "Couldn't open file '%s'\n", _path );
		return {};
	}

	std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

	Memory mem;
	mem.data = new unsigned char[ buf.size() ];
	mem.size = static_cast<unsigned int>( buf.size() );

	memcpy( mem.data, buf.data(), buf.size() );

	m_numLoadedFiles++;
	return mem;
}

void wv::MemoryDevice::freeMemory( Memory* _memory )
{
	if ( !_memory->data )
		return;

	delete _memory->data;
	*_memory = {};
	m_numLoadedFiles--;
}

std::string wv::MemoryDevice::loadString( const char* _path )
{
	Memory mem = loadFromFile( _path );
	std::string str( (const char*)mem.data, mem.size );
	freeMemory( &mem );
    return str;
}

wv::TextureMemory wv::MemoryDevice::loadTextureData( const char* _path )
{
	TextureMemory mem;

	m_numLoadedFiles++;
	stbi_set_flip_vertically_on_load( 0 );
	mem.data = reinterpret_cast<uint8_t*>( stbi_load( _path, &mem.width, &mem.height, &mem.numChannels, 0 ) );

	if ( !mem.data )
	{
		printf( "Failed to load texture %s\n", _path );
		unloadTextureData( &mem );
		return {}; // empty memory object
	}
	
	printf( "Loaded texture '%s' (%ix%i @ %ibpp)\n", _path, mem.width, mem.height, mem.numChannels * 8 );
	mem.size = mem.height * mem.numChannels * mem.width * mem.numChannels;
	return mem;
}

void wv::MemoryDevice::unloadTextureData( TextureMemory* _memory )
{
	stbi_image_free( _memory->data );
	*_memory = {};
	m_numLoadedFiles--;
}

wv::Mesh* wv::MemoryDevice::loadModel( const char* _path, bool _binary )
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	Memory gltf = loadFromFile( "res/meshes/cube.gltf" );
	std::string source{ (char*)gltf.data, gltf.size };

	nlohmann::json json = nlohmann::json::parse( source );

	std::vector<Memory> buffers;
	for ( auto& buffer : json["buffers"] )
	{
		std::string path = "res/meshes/" + (std::string)buffer[ "uri" ];
		buffers.push_back( loadFromFile( path.c_str() ) );
	}

	for ( auto& scene : json["scenes"] )
	{
		for ( int node : scene[ "nodes" ] )
		{
			
		}
	}

    return nullptr;
}

wv::Pipeline* wv::MemoryDevice::loadShaderPipeline( const std::string& _path )
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	std::string yaml = loadString( _path.c_str() );
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
	pipelineDesc.name = source.c_str();
	pipelineDesc.type = wv::WV_PIPELINE_GRAPHICS;
	pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
	pipelineDesc.shaders = shaders;
	pipelineDesc.uniformBlocks = blocks;
	pipelineDesc.textureUniforms = textureUniforms;

	printf( "Loaded shader '%s'\n", _path.c_str() );
	pipeline = device->createPipeline( &pipelineDesc );

    return pipeline;
}
