#pragma once

#include <wv/types.h>
#include <wv/graphics/types.h>
#include <wv/graphics/Texture.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct RenderTargetDesc
	{
		int width = 0;
		int height = 0;
		
		wv::TextureDesc* pTextureDescs = nullptr;
		int numTextures = 0;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct RenderTarget
	{
		wv::Handle fbHandle = 0;
		wv::Handle rbHandle = 0;

		wv::TextureID* pTextureIDs = nullptr;
		int numTextures = 0;

		int width = 0;
		int height = 0;

		void* pPlatformData = nullptr;

	};

}