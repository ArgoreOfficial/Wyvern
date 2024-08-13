#include "Shader.h"

#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/MemoryDevice.h>

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

	printf( "loaded %s\n", m_name.c_str() );

	iResource::load( _pFileSystem );
}

void wv::cShader::unload( cFileSystem* _pFileSystem )
{
	iResource::unload( _pFileSystem );
}

void wv::cShader::create( iGraphicsDevice* _pGraphicsDevice )
{
	_pGraphicsDevice->createShader( this, m_type );
	_pGraphicsDevice->compileShader( this );

	iResource::create( _pGraphicsDevice );
}

void wv::cShader::destroy( iGraphicsDevice* _pGraphicsDevice )
{
	_pGraphicsDevice->destroyShader( this );

	iResource::destroy( _pGraphicsDevice );
}
