#pragma once

#include <wv/Types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct TextureMemory;

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

	struct TextureDesc
	{
		TextureChannels channels = WV_TEXTURE_CHANNELS_RGB;
		TextureFormat format = WV_TEXTURE_FORMAT_BYTE;
		int width = 0;
		int height = 0;
		TextureMemory* memory = nullptr;
		bool generateMipMaps = false;
	};

	class Texture
	{
	public:
		wv::Handle handle;
		int width;
		int height;

	};

}
