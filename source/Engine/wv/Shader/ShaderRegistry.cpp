#include "ShaderRegistry.h"

#include <wv/Memory/MemoryDevice.h>

#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Auxiliary/fkYAML/node.hpp>
#include <wv/Shader/ShaderProgram.h>

wv::cShader* wv::cShaderRegistry::loadShader( eShaderType _type, const std::string& _name )
{
	cShader* shader = static_cast<cShader*>( getLoadedResource( _name ) );
	if ( shader )
	{
		shader->incrNumUsers();
		return shader;
	}

	std::wstring fullPath = m_pFileSystem->getFullPath( _name );
	if ( fullPath == L"" ) // file doesn't exist
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Couldn't find file '%s'\n", _name.c_str() );
		return nullptr;
	}

	std::string shaderSource = m_pFileSystem->loadString( _name );

	shader = m_pGraphicsDevice->createShader( _type, _name ); 
	shader->setSource( shaderSource );
	m_pGraphicsDevice->compileShader( shader );

	addResource( shader );

    return shader;
}

wv::cShaderProgram* wv::cShaderRegistry::loadProgramFromWShader( const std::string& _name )
{
	cShaderProgram* program = static_cast<cShaderProgram*>( getLoadedResource( _name ) );
	if ( program )
	{
		program->incrNumUsers();
		return program;
	}

	std::string yaml = m_pFileSystem->loadString( _name );
	fkyaml::node root = fkyaml::node::deserialize( yaml );

	std::string source = root[ "source" ].get_value<std::string>();

	cShader* vs = loadShader( WV_SHADER_TYPE_VERTEX,   "res/shaders/" + source + "_vs.glsl" );
	cShader* fs = loadShader( WV_SHADER_TYPE_FRAGMENT, "res/shaders/" + source + "_fs.glsl" );

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

	std::vector<Uniform> textureUniforms;
	unsigned int textureCounter = 0;
	if ( root[ "textures" ].is_sequence() )
	{
		for ( auto& texture : root[ "textures" ] )
		{
			wv::Uniform u{ textureCounter, 0, texture.get_value<std::string>() };
			textureUniforms.push_back( u );
			textureCounter++;
		}
	}

	program = m_pGraphicsDevice->createProgram( _name );
	program->addShader( vs );
	program->addShader( fs );
	m_pGraphicsDevice->linkProgram( program, blocks, textureUniforms );

	addResource( program );
	
	return program;
}

void wv::cShaderRegistry::unloadShaderProgram( cShaderProgram* _program )
{
	std::vector<cShader*> shaders = _program->getShaders();
	findAndUnloadResource( _program );

	for ( int i = 0; i < shaders.size(); i++ )
		findAndUnloadResource( shaders[ i ] );
}
