#include "TextureResource.h"

#include <wv/Engine/Engine.h>
#include <wv/Graphics/GraphicsDevice.h>
#include <wv/Memory/FileSystem.h>
#include <wv/Memory/Memory.h>

#ifdef WV_PLATFORM_WINDOWS
#include <wv/Auxiliary/stb_image.h>

#include <locale>
#include <codecvt>
#endif

void wv::cTextureResource::load( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	if ( m_name == "" ) // no file should be loaded
	{
		setComplete( true );
		return;
	}

	if ( m_path == "" )
		m_path = m_name;

	sTextureDesc desc;
	wv::Memory* mem = _pFileSystem->loadMemory( m_path );

#ifdef WV_PLATFORM_WINDOWS
	stbi_set_flip_vertically_on_load( 0 );
	m_pData = reinterpret_cast<uint8_t*>( stbi_load_from_memory( mem->data, mem->size, &desc.width, &desc.height, &desc.numChannels, 0 ) );
#endif
	_pFileSystem->unloadMemory( mem );
		
	if ( !m_pData )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", m_path.c_str() );
		WV_FREE( m_pData );
		return;
	}

	m_dataSize = desc.width * desc.numChannels * desc.width * desc.numChannels;
	
	desc.filtering = m_filtering;
	desc.generateMipMaps = true;
	desc.channels = (TextureChannels)desc.numChannels;
	
	m_textureID = _pLowLevelGraphics->createTexture( desc );
	_pLowLevelGraphics->bufferTextureData( m_textureID, m_pData, desc.generateMipMaps );
	m_pData = nullptr; // move ownership

	auto onCompleteCallback = []( void* _c ) 
		{ 
			cTextureResource* tex = (cTextureResource*)_c;
			sTexture& texObject = wv::cEngine::get()->graphics->m_textures.at( tex->m_textureID );

			tex->setComplete( true ); 
		#ifdef WV_PLATFORM_WINDOWS
			stbi_image_free( texObject.pData );
		#endif
			texObject.pData = nullptr;
		};

	_pLowLevelGraphics->queueAddCallback( onCompleteCallback, (void*)this );
}

void wv::cTextureResource::unload( cFileSystem* /*_pFileSystem*/, IGraphicsDevice* _pLowLevelGraphics )
{
	if( m_pData )
	{
		WV_FREE( m_pData );
		m_pData = nullptr;
	}

	m_dataSize = 0;
	_pLowLevelGraphics->destroyTexture( m_textureID );
}
