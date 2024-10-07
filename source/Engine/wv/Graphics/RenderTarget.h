#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>
#include <wv/Graphics/Texture.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sRenderTargetDesc
	{
		int width = 0;
		int height = 0;
		
		wv::sTextureDesc* pTextureDescs = nullptr;
		int numTextures = 0;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sRenderTarget
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