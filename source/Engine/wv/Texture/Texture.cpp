#include "Texture.h"

#include <wv/Memory/FileSystem.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Engine/Engine.h>

#ifdef WV_PLATFORM_WINDOWS
#include <wv/Auxiliary/stb_image.h>

#include <locale>
#include <codecvt>
#endif

void wv::Texture::load( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{
#ifdef WV_PLATFORM_WINDOWS
	if ( m_name == "" ) // no file should be loaded
	{
		setComplete( true );
		return;
	}

	if ( m_path == "" )
		m_path = _pFileSystem->getFullPath( m_name );

	stbi_set_flip_vertically_on_load( 0 );
	m_pData = reinterpret_cast<uint8_t*>( stbi_load( m_path.c_str(), &m_width, &m_height, &m_numChannels, 0 ) );

	if ( !m_pData )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", m_path.c_str() );
		delete m_pData;

		return;
	}

	m_dataSize = m_height * m_numChannels * m_width * m_numChannels;
	

	TextureDesc desc;
	desc.filtering = m_filtering;
	wv::cCommandBuffer& cmdBuffer = _pGraphicsDevice->getCommandBuffer();
	cmdBuffer.push( WV_GPUTASK_CREATE_TEXTURE, (void**)this, &desc ); // hack

	auto onCompleteCallback = []( void* _c ) { ( (iResource*)( _c ) )->setComplete( true ); };
	cmdBuffer.callback.bind( onCompleteCallback );
	cmdBuffer.callbacker = (void*)this;

	_pGraphicsDevice->submitCommandBuffer( cmdBuffer );
	if ( _pGraphicsDevice->getThreadID() == std::this_thread::get_id() )
		_pGraphicsDevice->executeCommandBuffer( cmdBuffer );

#else
	printf( "wv::Texture::load unimplemented\n" );
#endif
}

void wv::Texture::unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{

}
