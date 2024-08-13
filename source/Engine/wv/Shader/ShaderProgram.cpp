#include "ShaderProgram.h"

#include <wv/Memory/MemoryDevice.h>
#include <wv/Auxiliary/json.hpp>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Shader/ShaderRegistry.h>

void wv::cShaderProgram::load( cFileSystem* _pFileSystem )
{
	std::string jsonSource = _pFileSystem->loadString( m_name );
	nlohmann::json root = nlohmann::json::parse( jsonSource );

	std::string source = root.value<std::string>( "source", "source/unlit" );

	cShader* vs = m_pShaderRegistry->loadShader( WV_SHADER_TYPE_VERTEX, "res/shaders/" + source + "_vs.glsl" );
	cShader* fs = m_pShaderRegistry->loadShader( WV_SHADER_TYPE_FRAGMENT, "res/shaders/" + source + "_fs.glsl" );

	addShader( vs );
	addShader( fs );

	while ( !vs->isLoaded() || !fs->isLoaded() ) { }

	int blockCounter = 0;
	for ( auto& block : root[ "blocks" ] )
	{
		UniformBlockDesc blockDesc{};

		blockDesc.name = block.value<std::string>( "block", "" );
		if( blockDesc.name == "" )
		{
			Debug::Print( Debug::WV_PRINT_ERROR, "Block name is empty\n" );
			continue;
		}

		for ( auto& uniform : block[ "uniforms" ] )
		{
			std::string uniformName = uniform.value<std::string>( "uniform", "" );
			if( uniformName == "" )
			{
				Debug::Print( Debug::WV_PRINT_ERROR, "Uniform name is empty\n" );
				continue;
			}

			Uniform u{ (unsigned int)blockCounter, 0, uniformName };
			blockDesc.uniforms.push_back( u );
		}

		blockCounter++;
		m_uniformBlockDescs.push_back( blockDesc );
	}

	unsigned int textureCounter = 0;
	if ( !root[ "textures" ].empty() )
	{
		for ( auto& texture : root[ "textures" ].items() )
		{
			wv::Uniform u{ textureCounter, 0, texture.value() };
			m_textureUniforms.push_back( u );
			textureCounter++;
		}
	}

	iResource::load( _pFileSystem );
}

void wv::cShaderProgram::unload( cFileSystem* _pFileSystem )
{
	iResource::unload( _pFileSystem );
}

void wv::cShaderProgram::create( iGraphicsDevice* _pGraphicsDevice )
{
	for ( size_t i = 0; i < m_shaders.size(); i++ )
	{
		if ( !m_shaders[ i ]->getHandle() )
			m_shaders[ i ]->create( _pGraphicsDevice );
	}

	_pGraphicsDevice->createProgram( this, m_name );
	_pGraphicsDevice->linkProgram( this, m_uniformBlockDescs, m_textureUniforms );

	iResource::create( _pGraphicsDevice );
}

void wv::cShaderProgram::destroy( iGraphicsDevice* _pGraphicsDevice )
{
	iResource::destroy( _pGraphicsDevice );
}
