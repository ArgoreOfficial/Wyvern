#include "Texture.h"

#include <wv/Memory/FileSystem.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Engine/Engine.h>

#ifdef WV_PLATFORM_WINDOWS
#include <wv/Auxiliary/stb_image.h>

#include <locale>
#include <codecvt>
#endif

void wv::cTextureResource::load( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{
#ifdef WV_PLATFORM_WINDOWS
	if ( m_name == "" ) // no file should be loaded
	{
		setComplete( true );
		return;
	}

	if ( m_path == "" )
		m_path = _pFileSystem->getFullPath( m_name );

	sTextureDesc desc;

	stbi_set_flip_vertically_on_load( 0 );
	m_pData = reinterpret_cast<uint8_t*>( stbi_load( m_path.c_str(), &desc.width, &desc.height, &desc.numChannels, 0 ) );

	if ( !m_pData )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", m_path.c_str() );
		delete m_pData;

		return;
	}

	m_dataSize = desc.width * desc.numChannels * desc.width * desc.numChannels;
	
	desc.filtering = m_filtering;
	uint32_t cmdBuffer = _pGraphicsDevice->getCommandBuffer();
	
	struct
	{
		sTexture* tex;
		void* pData;
		bool generateMipMaps = false;
	} bufferData;
	bufferData.tex = &m_texture;
	bufferData.pData = m_pData;

	_pGraphicsDevice->bufferCommand( cmdBuffer, WV_GPUTASK_CREATE_TEXTURE, (void**)&m_texture, &desc ); // hack
	_pGraphicsDevice->bufferCommand( cmdBuffer, WV_GPUTASK_BUFFER_TEXTURE_DATA, &bufferData );

	auto onCompleteCallback = []( void* _c ) { ( (iResource*)( _c ) )->setComplete( true ); };

	_pGraphicsDevice->setCommandBufferCallback( cmdBuffer, onCompleteCallback, (void*)this );

	_pGraphicsDevice->submitCommandBuffer( cmdBuffer );
	if ( _pGraphicsDevice->getThreadID() == std::this_thread::get_id() )
		_pGraphicsDevice->executeCommandBuffer( cmdBuffer );

#else
	printf( "wv::cTextureResource::load unimplemented\n" );
#endif
}

void wv::cTextureResource::unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{

}
