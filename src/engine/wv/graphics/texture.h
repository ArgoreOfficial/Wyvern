#pragma once

#include <wv/types.h>
#include <wv/graphics/types.h>
#include <wv/resource/resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	enum TextureChannels
	{
		WV_TEXTURE_CHANNELS_R = 1,
		WV_TEXTURE_CHANNELS_RG,
		WV_TEXTURE_CHANNELS_RGB,
		WV_TEXTURE_CHANNELS_RGBA
	};

	enum TextureFormat
	{
		WV_TEXTURE_FORMAT_BYTE,
		WV_TEXTURE_FORMAT_INT,
		WV_TEXTURE_FORMAT_FLOAT
	};

	enum TextureFiltering
	{
		WV_TEXTURE_FILTER_NEAREST,
		WV_TEXTURE_FILTER_LINEAR,
	};

	class TextureResource;

	struct TextureDesc
	{
		TextureChannels  channels  = WV_TEXTURE_CHANNELS_RGB;
		TextureFormat    format    = WV_TEXTURE_FORMAT_BYTE;
		TextureFiltering filtering = WV_TEXTURE_FILTER_NEAREST;
		int width = 0;
		int height = 0;
		int numChannels = 0;
		bool generateMipMaps = false;
	};

	struct PlatformTextureData;

	struct Texture
	{
		TextureFiltering m_filtering;

		wv::Handle textureObjectHandle = 0; // opengl specific
		uint64_t textureHandle = 0; // opengl specific
		
		uint8_t* pData = nullptr;
		unsigned int dataSize = 0;

		int width  = 0;
		int height = 0;
		int numChannels = 0;

		PlatformTextureData* pPlatformData = nullptr;
	};
}
