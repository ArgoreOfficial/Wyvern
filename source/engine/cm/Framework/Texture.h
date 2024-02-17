#pragma once

/*
 *
 * Texture.h
 *
 * Type definition for Texture objects
 *
 */

namespace cm
{
	typedef unsigned int hTexture;

	struct sTexture2D
	{
		int width;
		int height;
		int num_channels;
		hTexture handle;
	};
}

