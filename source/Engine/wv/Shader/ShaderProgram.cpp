#include "ShaderProgram.h"

#include <wv/Memory/MemoryDevice.h>

#include <wv/Auxiliary/json/json11.hpp>

#include <wv/Device/GraphicsDevice.h>

void wv::cShaderProgram::load( cFileSystem* _pFileSystem )
{
#ifdef WV_PLATFORM_WINDOWS
	Debug::Print( Debug::WV_PRINT_DEBUG, "Loading Shader '%s'", m_name );

	m_vsSource.data = _pFileSystem->loadMemory( "res/shaders/" + m_name + "_vs.glsl" );
	m_fsSource.data = _pFileSystem->loadMemory( "res/shaders/" + m_name + "_fs.glsl" );

	iResource::load( _pFileSystem );
#endif
}

void wv::cShaderProgram::unload( cFileSystem* _pFileSystem )
{
	iResource::unload( _pFileSystem );
}

void wv::cShaderProgram::create( iGraphicsDevice* _pGraphicsDevice )
{
	sShader* vs = _pGraphicsDevice->createShader( WV_SHADER_TYPE_VERTEX,   &m_vsSource );
	sShader* fs = _pGraphicsDevice->createShader( WV_SHADER_TYPE_FRAGMENT, &m_fsSource );

	sShaderProgramDesc desc;
	desc.name = m_name;
	desc.shaders.push_back( vs );
	desc.shaders.push_back( fs );

	m_pProgram = _pGraphicsDevice->createProgram( &desc );
	
	iResource::create( _pGraphicsDevice );
}

void wv::cShaderProgram::destroy( iGraphicsDevice* _pGraphicsDevice )
{
	iResource::destroy( _pGraphicsDevice );
}

void wv::cShaderProgram::use( iGraphicsDevice* _pGraphicsDevice )
{
	_pGraphicsDevice->useProgram( m_pProgram );
}

wv::cShaderBuffer* wv::cShaderProgram::getShaderBuffer( const std::string& _name )
{
	for( auto& buf : m_pProgram->shaderBuffers )
	{
		if( buf->name == _name )
			return buf;
	}

	return nullptr;
}