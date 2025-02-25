#include "texture_resource.h"

#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>
#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>

#include <wv/job/job_system.h>
#include <wv/resource/resource_registry.h>

#ifdef WV_PLATFORM_WINDOWS
#include <auxiliary/stb_image.h>

#include <locale>
#include <codecvt>
#endif

void wv::TextureResource::load( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice )
{
	if ( m_name == "" ) // no file should be loaded
	{
		setComplete( true );
		return;
	}

	if ( m_path == "" )
		m_path = m_name;


	wv::Memory* mem = _pFileSystem->loadMemory( m_path );

#ifdef WV_PLATFORM_WINDOWS
	stbi_set_flip_vertically_on_load( 0 );
	m_pData = reinterpret_cast<uint8_t*>( stbi_load_from_memory( mem->data, mem->size, &m_width, &m_height, &m_numChannels, 0 ) );
#endif
	_pFileSystem->unloadMemory( mem );

	if ( !m_pData )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", m_path.c_str() );
		WV_FREE( m_pData );
		return;
	}

	JobSystem* pJobSystem = Engine::get()->m_pJobSystem;

	Job::JobFunction_t fptr = [&]( void* _pUserData )
		{
			Engine* app = Engine::get();
			IGraphicsDevice* pGraphicsDevice = app->graphics;

			TextureDesc desc;
			desc.width       = m_width;
			desc.height      = m_height;
			desc.numChannels = m_numChannels;
			desc.filtering   = m_filtering;
			desc.generateMipMaps = true;
			desc.channels = (TextureChannels)desc.numChannels;

			m_textureID = pGraphicsDevice->createTexture( desc );
			
			if ( m_pData )
			{
				pGraphicsDevice->bufferTextureData( m_textureID, m_pData, desc.generateMipMaps );
				free( m_pData ); // because of stbi
				m_pData = nullptr;
			}
			
			setComplete( true );
		};

	Job* job = pJobSystem->createJob( JobThreadType::kRENDER, fptr );
	pJobSystem->submit( { job } );
}

void wv::TextureResource::unload( IFileSystem* /*_pFileSystem*/, IGraphicsDevice* _pGraphicsDevice )
{
	if( m_pData )
	{
		WV_FREE( m_pData );
		m_pData = nullptr;
	}

	if( m_textureID.is_valid() )
	{
		JobSystem* pJobSystem = Engine::get()->m_pJobSystem;

		TextureID id = m_textureID; // lambdas can only capture local variables
		Job::JobFunction_t fptr = [=]( void* _pUserData )
			{
				_pGraphicsDevice->destroyTexture( id );
			};

		Job* job = pJobSystem->createJob( JobThreadType::kRENDER, fptr, Engine::get()->m_pResourceRegistry->getResourceFence() );

		pJobSystem->submit( { job } );
	}
}
