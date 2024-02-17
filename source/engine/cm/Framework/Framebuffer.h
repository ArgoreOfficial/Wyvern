#pragma once

/*
 *
 * Framebuffer.h
 * 
 * Type definitions for framebuffer objects
 * 
 */

#include <vector>


namespace cm
{
	typedef unsigned int hFramebuffer;
	typedef unsigned int hFramebufferTexture;
	
	enum eFramebufferFormat
	{
		FramebufferFormat_Ri,
		FramebufferFormat_RGi,
		FramebufferFormat_RGBi,
		FramebufferFormat_RGBAi,
		FramebufferFormat_Rf,
		FramebufferFormat_RGf,
		FramebufferFormat_RGBf,
		FramebufferFormat_RGBAf
	};

	enum eFramebufferType
	{
		FramebufferType_Color,
		FramebufferType_Depth,
		FramebufferType_Stencil,
	};

	struct sFramebufferTexture // TODO: change to cm::sTexture2D ? 
	{
		hFramebufferTexture handle;
		eFramebufferFormat format;
		eFramebufferType type;
		int attachment_slot;
		int width;
		int height;
	};

	struct sFramebuffer
	{
		hFramebuffer handle;
		std::vector<sFramebufferTexture> textures;

	};
}