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
		TextureFormat_R,
		TextureFormat_RG,
		TextureFormat_RGB,
		TextureFormat_RGBA,
		TextureFormat_Ri,
		TextureFormat_RGi,
		TextureFormat_RGBi,
		TextureFormat_RGBAi,
		TextureFormat_Rf,
		TextureFormat_RGf,
		TextureFormat_RGBf,
		TextureFormat_RGBAf,
	};

	enum eTextureType
	{
		TextureType_Color,
		TextureType_Depth,
		TextureType_Stencil,
	};

	enum eTextureTarget
	{
		TextureTarget_Texture2D,
		TextureTarget_Texture2DMultisample
	};

	struct sTexture2D
	{
		eTextureFormat format;
		eTextureType   type;
		eTextureTarget target;
		hTexture       handle;
		int width;
		int height;
		int num_channels;
		int attachment_slot;
		std::string name;
	};
}

