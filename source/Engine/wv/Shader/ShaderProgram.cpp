#include "ShaderProgram.h"

#include <wv/Memory/MemoryDevice.h>
#include <wv/Auxiliary/fkYAML/node.hpp>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Shader/ShaderRegistry.h>

void wv::cShaderProgram::load( cFileSystem* _pFileSystem )
{
	std::string yaml = _pFileSystem->loadString( m_name );
	fkyaml::node root = fkyaml::node::deserialize( yaml );

	std::string source = root[ "source" ].get_value<std::string>();

	cShader* vs = m_pShaderRegistry->loadShader( WV_SHADER_TYPE_VERTEX, "res/shaders/" + source + "_vs.glsl" );
	cShader* fs = m_pShaderRegistry->loadShader( WV_SHADER_TYPE_FRAGMENT, "res/shaders/" + source + "_fs.glsl" );

	addShader( vs );
	addShader( fs );

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
		m_uniformBlockDescs.push_back( blockDesc );
	}

	unsigned int textureCounter = 0;
	if ( root[ "textures" ].is_sequence() )
	{
		for ( auto& texture : root[ "textures" ] )
		{
			wv::Uniform u{ textureCounter, 0, texture.get_value<std::string>() };
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

void wv::cShaderProgram::create()
{
	for ( size_t i = 0; i < m_shaders.size(); i++ )
	{
		if ( !m_shaders[ i ]->getHandle() )
			m_shaders[ i ]->create();
	}

	m_pGraphicsDevice->createProgram( this, m_name );
	m_pGraphicsDevice->linkProgram( this, m_uniformBlockDescs, m_textureUniforms );

	iResource::create();
}

void wv::cShaderProgram::destroy()
{
	iResource::destroy();
}
