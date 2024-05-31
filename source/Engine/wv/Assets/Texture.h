#pragma once

#include <wv/Types.h>

namespace wv
{
	enum TextureChannels
	{
		WV_TEXTURE_CHANNELS_R,
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
		const char* filepath;
		TextureChannels channels;
		TextureFormat format;
	};

	class Texture
	{
	public:
		wv::Handle handle;

	};
}