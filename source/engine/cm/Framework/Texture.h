#pragma once

/*
 *
 * Texture.h
 *
 * Type definition for Texture objects
 *
 */

#include <string>

namespace cm
{
	typedef unsigned int hTexture;

	enum eTextureFormat
	{
		FramebufferFormat_R,
		FramebufferFormat_RG,
		FramebufferFormat_RGB,
		FramebufferFormat_RGBA,
		FramebufferFormat_Ri,
		FramebufferFormat_RGi,
		FramebufferFormat_RGBi,
		FramebufferFormat_RGBAi,
		FramebufferFormat_Rf,
		FramebufferFormat_RGf,
		FramebufferFormat_RGBf,
		FramebufferFormat_RGBAf,
	};

	enum eTextureType
	{
		FramebufferType_Color,
		FramebufferType_Depth,
		FramebufferType_Stencil,
	};

	struct sTexture2D
	{
		eTextureFormat format;
		eTextureType type;
		hTexture handle;
		int width;
		int height;
		int num_channels;
		int attachment_slot;
		std::string name;
	};
}

