#pragma once

#include <wv/read_through_cache.h>
#include <wv/resource_id.h>

#include <filesystem>

namespace wv {

struct TextureData
{
	int width    = 0;
	int height   = 0;
	int channels = 0;

	uint8_t* data = nullptr;
	size_t dataSize = 0;
};

class TextureAsset
{
public:
	TextureAsset() = default;
	TextureAsset( const std::filesystem::path& _path );
	~TextureAsset();

	TextureData deserialize( const std::filesystem::path& _path );
	
	void initialize( const TextureData& _texture );

	ResourceID getGPUAllocation() const { return m_gpuAllocation; }
	uint32_t   getImageSlot()     const { return m_imageSlot; }

private:
	std::filesystem::path m_path;

	TextureData m_data{};
	ResourceID m_gpuAllocation{};
	uint32_t m_imageSlot = 0;
};

class TextureManager : public ReadThroughCache<TextureAsset> { };

}