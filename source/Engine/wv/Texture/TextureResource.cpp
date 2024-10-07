#include "TextureResource.h"

#include <wv/Memory/FileSystem.h>
#include <wv/Graphics/Graphics.h>
#include <wv/Engine/Engine.h>

#ifdef WV_PLATFORM_WINDOWS
#include <wv/Auxiliary/stb_image.h>

#include <locale>
#include <codecvt>
#endif

void wv::cTextureResource::load( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
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
	wv::Memory* mem = _pFileSystem->loadMemory( m_path );

	stbi_set_flip_vertically_on_load( 0 );
	m_pData = reinterpret_cast<uint8_t*>( stbi_load_from_memory( mem->data, mem->size, &desc.width, &desc.height, &desc.numChannels, 0 ) );
	_pFileSystem->unloadMemory( mem );
		
	if ( !m_pData )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", m_path.c_str() );
		delete m_pData;
		return;
	}

	m_dataSize = desc.width * desc.numChannels * desc.width * desc.numChannels;
	
	desc.filtering = m_filtering;
	desc.generateMipMaps = true;
	desc.channels = (TextureChannels)desc.numChannels;
	CmdBufferID cmdBuffer = _pLowLevelGraphics->getCommandBuffer();
	
	struct
	{
		TextureID* tex;
		void* pData;
		bool generateMipMaps;
	} bufferData;
	bufferData.generateMipMaps = desc.generateMipMaps;

	bufferData.tex = &m_textureID;
	bufferData.pData = m_pData;
	m_pData = nullptr; // move ownership

	m_textureID = _pLowLevelGraphics->cmdCreateTexture( cmdBuffer, desc );
	_pLowLevelGraphics->cmd( cmdBuffer, WV_GPUTASK_BUFFER_TEXTURE_DATA, &bufferData );

	auto onCompleteCallback = []( void* _c ) 
		{ 
			cTextureResource* tex = (cTextureResource*)_c;
			sTexture& texObject = wv::cEngine::get()->graphics->m_textures.get( tex->m_textureID );

			tex->setComplete( true ); 
			stbi_image_free( texObject.pData );
			texObject.pData = nullptr;
		};

	_pLowLevelGraphics->setCommandBufferCallback( cmdBuffer, onCompleteCallback, (void*)this );

	_pLowLevelGraphics->submitCommandBuffer( cmdBuffer );
	if ( _pLowLevelGraphics->getThreadID() == std::this_thread::get_id() )
		_pLowLevelGraphics->executeCommandBuffer( cmdBuffer );

#else
	printf( "wv::cTextureResource::load unimplemented\n" );
#endif
}

void wv::cTextureResource::unload( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
{
	if( m_pData )
	{
		delete m_pData;
		m_pData = nullptr;
	}

	m_dataSize = 0;
	_pLowLevelGraphics->destroyTexture( m_textureID );
}
