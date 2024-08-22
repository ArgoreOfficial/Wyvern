#include "ShaderProgram.h"

#include <wv/Memory/FileSystem.h>

#include <wv/Auxiliary/json/json11.hpp>

#include <wv/Device/GraphicsDevice.h>

void wv::cShaderProgram::load( cFileSystem* _pFileSystem )
{
	Debug::Print( Debug::WV_PRINT_DEBUG, "Loading Shader '%s'\n", m_name.c_str() );

	std::string basepath = "res/shaders/";
	std::string ext;

#ifdef WV_PLATFORM_WINDOWS
	ext = ".glsl";
#elif defined( WV_PLATFORM_PSVITA )
	ext = "_cg.gxp";
	basepath += "psvita/";
#endif

	m_vsSource.data = _pFileSystem->loadMemory( basepath + m_name + "_vs" + ext );
	m_fsSource.data = _pFileSystem->loadMemory( basepath + m_name + "_fs" + ext );

	iResource::load( _pFileSystem );
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
	
	wv::sVertexAttribute attributes[] = {
			{ "aPosition",  3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ "aNormal",    3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ "aTangent",   3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ "aColor",     4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ "aTexCoord0", 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};
	wv::sVertexLayout layout;
	layout.elements = attributes;
	layout.numElements = 5;

	desc.pVertexLayout = &layout;
	desc.pVertexShader = vs;
	desc.pFragmentShader = fs;
	
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