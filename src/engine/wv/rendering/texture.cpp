#include "texture.h"

#include <wv/application.h>
#include <wv/rendering/renderer.h>
#include <wv/filesystem/file_system.h>

#define STB_IMAGE_IMPLEMENTATION
#include <auxiliary/stb_image.h>

wv::TextureAsset::TextureAsset( const uint8_t* _data, size_t _size )
{
	initialize( deserialize( _data, _size ) );
}

wv::TextureAsset::TextureAsset( const std::filesystem::path& _path )
{
	initialize( deserialize( _path ) );
}

wv::TextureAsset::~TextureAsset()
{
	Renderer* renderer = Application::getSingleton()->getRenderer();
	renderer->deallocateImage( m_gpuAllocation );
}

wv::TextureData wv::TextureAsset::deserialize( const uint8_t* _data, size_t _size )
{
	TextureData textureData{};

	textureData.data = stbi_load_from_memory( _data, _size, &textureData.width, &textureData.height, &textureData.channels, 4 );

	return textureData;
}

wv::TextureData wv::TextureAsset::deserialize( const std::filesystem::path& _path )
{
	IFileSystem* fs = Application::getSingleton()->getFileSystem();

	std::vector<uint8_t> data = fs->loadEntireFile( _path );

	return deserialize( data.data(), data.size() );
}

void wv::TextureAsset::initialize( const TextureData& _texture )
{
	Renderer* renderer = Application::getSingleton()->getRenderer();
	
	if ( _texture.data && _texture.width > 0 && _texture.height > 0 )
	{
		m_gpuAllocation = renderer->allocateImage( _texture.data, _texture.width, _texture.height, false );
		m_imageSlot     = renderer->storeImage( m_gpuAllocation, SamplerState::WV_SAMPLER_NEAREST );
	}

	m_data = _texture;

	// deallocate cpu copy
	stbi_image_free( m_data.data );
	m_data.data = nullptr;
}
