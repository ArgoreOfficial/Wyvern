#include "Texture.h"

#include <wv/Memory/MemoryDevice.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Auxiliary/stb_image.h>
#include <locale>
#include <codecvt>

void wv::Texture::load( cFileSystem* _pFileSystem )
{
	if ( m_name == "" ) // no file should be loaded
	{
		m_loaded = true;
		return;
	}

	if ( m_path == L"" )
		m_path = _pFileSystem->getFullPath( m_name );

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string path = converter.to_bytes( m_path ); // convert wstring to string

	stbi_set_flip_vertically_on_load( 0 );
	m_pData = reinterpret_cast<uint8_t*>( stbi_load( path.c_str(), &m_width, &m_height, &m_numChannels, 0 ) );

	if ( !m_pData )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", path.c_str() );
		delete m_pData;

		return;
	}

	m_dataSize = m_height * m_numChannels * m_width * m_numChannels;
	
	iResource::load( _pFileSystem );
}

void wv::Texture::unload( cFileSystem* _pFileSystem )
{

	iResource::unload( _pFileSystem );
}

void wv::Texture::create( iGraphicsDevice* _pGraphicsDevice )
{

	TextureDesc desc;
	_pGraphicsDevice->createTexture( this, &desc );

	iResource::create( _pGraphicsDevice );
}

void wv::Texture::destroy( iGraphicsDevice* _pGraphicsDevice )
{
	iResource::destroy( _pGraphicsDevice );
}
