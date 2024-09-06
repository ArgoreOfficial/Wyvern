#include "ShaderProgram.h"

#include <wv/Memory/MemoryDevice.h>

#include <wv/Auxiliary/json/json11.hpp>

#include <wv/Device/GraphicsDevice.h>
#include <wv/Shader/ShaderRegistry.h>

void wv::cShaderProgram::load( cFileSystem* _pFileSystem )
{
#ifdef WV_PLATFORM_WINDOWS
	std::string jsonSource = _pFileSystem->loadString( m_name );
	std::string err;
	json11::Json root = json11::Json::parse( jsonSource, err );

	std::string source = root["source"].string_value();

	cShader* vs = m_pShaderRegistry->loadShader( WV_SHADER_TYPE_VERTEX, "res/shaders/" + source + "_vs.glsl" );
	cShader* fs = m_pShaderRegistry->loadShader( WV_SHADER_TYPE_FRAGMENT, "res/shaders/" + source + "_fs.glsl" );

	addShader( vs );
	addShader( fs );

	while ( !vs->isLoaded() || !fs->isLoaded() ) 
	{
	#ifdef WV_PLATFORM_WINDOWS
		Sleep( 1 );
	#endif
	}

	int blockCounter = 0;
	for ( auto& block : root[ "blocks" ].array_items() )
	{
		UniformBlockDesc blockDesc{};

		blockDesc.name = block[ "block" ].string_value();
		if( blockDesc.name == "" )
		{
			Debug::Print( Debug::WV_PRINT_ERROR, "Block name is empty\n" );
			continue;
		}

		for ( auto& uniform : block[ "uniforms" ].array_items() )
		{
			std::string uniformName = uniform[ "uniform" ].string_value();
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
	if ( !root[ "textures" ].array_items().empty() )
	{
		for ( auto& texture : root[ "textures" ].array_items() )
		{
			wv::Uniform u{ textureCounter, 0, texture.string_value() };
			m_textureUniforms.push_back( u );
			textureCounter++;
		}
	}

	iResource::load( _pFileSystem );
#endif
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
wv::UniformBlock* wv::cShaderProgram::getUniformBlock( const std::string& _name )
{
	auto search = m_uniformBlocks.find( _name );
	if( search == m_uniformBlocks.end() )
		return nullptr;

	return &m_uniformBlocks[ _name ];
}

void wv::cShaderProgram::addUniformBlock( const std::string& _name, const UniformBlock& _block )
{
	auto search = m_uniformBlocks.find( _name );
	if( search != m_uniformBlocks.end() )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Uniform Block '%s' already exists\n", _name.c_str() );
		return;
	}

	m_uniformBlocks[ _name ] = _block;
}
