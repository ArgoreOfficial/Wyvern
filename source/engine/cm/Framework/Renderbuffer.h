#pragma once

/*
 *
 * Renderbuffer.h
 * 
 * Type definitions for renderbuffer objects
 * 
 */

namespace cm
{
	typedef unsigned int hRenderbuffer;

	enum eRenderbufferType
	{
		RenderbufferType_Depth,
		RenderbufferType_Stencil,
	};

	struct sRenderbuffer
	{
		hRenderbuffer handle;
		eRenderbufferType type;
		int width;
		int height;
	};
}