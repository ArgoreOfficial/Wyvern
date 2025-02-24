#include "texture_resource.h"

#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>
#include <wv/memory/file_system.h>
#include <wv/memory/memory.h>

#include <wv/job/job_system.h>

#ifdef WV_PLATFORM_WINDOWS
#include <auxiliary/stb_image.h>

#include <locale>
#include <codecvt>
#endif

void wv::TextureResource::load( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	if ( m_name == "" ) // no file should be loaded
	{
		setComplete( true );
		return;
	}

	if ( m_path == "" )
		m_path = m_name;

	JobSystem* pJobSystem = Engine::get()->m_pJobSystem;

	Job::JobFunction_t fptr = []( void* _pUserData )
		{
			Engine* app = Engine::get();
			FileSystem* pFileSystem = app->m_pFileSystem;
			IGraphicsDevice* pGraphicsDevice = app->graphics;

			wv::TextureResource& _this = *(wv::TextureResource*)_pUserData;


			TextureDesc desc;
			wv::Memory* mem = pFileSystem->loadMemory( _this.m_path );

		#ifdef WV_PLATFORM_WINDOWS
			stbi_set_flip_vertically_on_load( 0 );
			_this.m_pData = reinterpret_cast<uint8_t*>( stbi_load_from_memory( mem->data, mem->size, &desc.width, &desc.height, &desc.numChannels, 0 ) );
		#endif
			pFileSystem->unloadMemory( mem );

			if( !_this.m_pData )
			{
				Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", _this.m_path.c_str() );
				WV_FREE( _this.m_pData );
				return;
			}

			_this.m_dataSize = desc.width * desc.numChannels * desc.width * desc.numChannels;

			desc.filtering = _this.m_filtering;
			desc.generateMipMaps = true;
			desc.channels = (TextureChannels)desc.numChannels;

			_this.m_textureID = pGraphicsDevice->createTexture( desc );
			pGraphicsDevice->bufferTextureData( _this.m_textureID, _this.m_pData, desc.generateMipMaps );
			_this.m_pData = nullptr; // move ownership

			Texture& texObject = wv::Engine::get()->graphics->m_textures.at( _this.m_textureID );

			_this.setComplete( true );
		#ifdef WV_PLATFORM_WINDOWS
			stbi_image_free( texObject.pData );
		#endif
			texObject.pData = nullptr;
		};

	Job* job = pJobSystem->createJob( JobThreadType::kRENDER, nullptr, nullptr, fptr, this );
	pJobSystem->submit( { job } );
}

void wv::TextureResource::unload( FileSystem* /*_pFileSystem*/, IGraphicsDevice* _pLowLevelGraphics )
{
	if( m_pData )
	{
		WV_FREE( m_pData );
		m_pData = nullptr;
	}

	m_dataSize = 0;
	_pLowLevelGraphics->destroyTexture( m_textureID );
}
