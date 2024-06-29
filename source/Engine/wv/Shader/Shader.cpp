#include "Shader.h"

#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/MemoryDevice.h>

/// TODO: BAD
#include <Windows.h>

wv::cShader::~cShader()
{
}

void wv::cShader::load( cFileSystem* _pFileSystem )
{
	std::wstring fullPath = _pFileSystem->getFullPath( m_name );
	if ( fullPath == L"" ) // file doesn't exist
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Couldn't find file '%s'\n", m_name.c_str() );
		return; // false;
	}

	m_path = fullPath;
	std::string shaderSource = _pFileSystem->loadString( m_name );
	setSource( shaderSource );

	Sleep( 400 );
	// addResource( shader );
}

void wv::cShader::unload( cFileSystem* _pFileSystem )
{

}

void wv::cShader::create()
{
	m_pGraphicsDevice->createShader( this, m_type );
	m_pGraphicsDevice->compileShader( this );
}

void wv::cShader::destroy()
{
	m_pGraphicsDevice->destroyShader( this );
}
