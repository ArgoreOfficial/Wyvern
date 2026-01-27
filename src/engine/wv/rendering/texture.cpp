#include "texture.h"

#include <wv/application.h>
#include <wv/rendering/renderer.h>
#include <wv/filesystem/file_system.h>

#define STB_IMAGE_IMPLEMENTATION
#include <auxiliary/stb_image.h>

wv::TextureAsset::TextureAsset( const std::filesystem::path& _path )
{
	TextureData data = deserialize( _path );
	initialize( data );
}

wv::TextureAsset::~TextureAsset()
{
	Renderer* renderer = Application::getSingleton()->getRenderer();
	renderer->deallocateImage( m_gpuAllocation );
}

wv::TextureData wv::TextureAsset::deserialize( const std::filesystem::path& _path )
{
	TextureData textureData{};

	IFileSystem* fs = Application::getSingleton()->getFileSystem();
	std::filesystem::path fullpath = fs->getFullPath( _path );

	textureData.data = stbi_load( fullpath.string().c_str(), &textureData.width, &textureData.height, &textureData.channels, 4 );
	
	// TODO: stbi_load_from_memory()

	return textureData;
}

void wv::TextureAsset::initialize( const TextureData& _texture )
{
	Renderer* renderer = Application::getSingleton()->getRenderer();
	m_gpuAllocation = renderer->allocateImage( _texture.data, _texture.width, _texture.height, false );
	m_imageSlot = renderer->storeImage( m_gpuAllocation, SamplerState::WV_SAMPLER_LINEAR );

	m_data = _texture;

	// deallocate cpu copy
	stbi_image_free( m_data.data );
	m_data.data = nullptr;
}
