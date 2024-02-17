#pragma once

/*
 *
 * Framebuffer.h
 * 
 * Type definitions for framebuffer objects
 * 
 */

#include <vector>
#include <cm/Framework/Renderbuffer.h>
#include <cm/Framework/Texture.h>

namespace cm
{
	typedef unsigned int hFramebuffer;
	typedef unsigned int hFramebufferTexture;

	struct sFramebuffer
	{
		hFramebuffer handle;
		std::vector<sTexture2D> textures;
		std::vector<sRenderbuffer> renderbuffers;
	};
}