#pragma once

#include <wv/types.h>

namespace wv {

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

struct GLTexture
{
	wv::GLHandle texture_handle = 0;
	uint32_t num_channels = 0;
	uint32_t width  = 0;
	uint32_t height = 0;
};


}