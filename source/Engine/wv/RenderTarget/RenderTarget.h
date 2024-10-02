#pragma once

#include <wv/Types.h>
#include <wv/Texture/Texture.h>

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

		wv::sTexture* pTextures = nullptr;
		int numTextures = 0;

		int width = 0;
		int height = 0;

		void* pPlatformData = nullptr;

	};

}