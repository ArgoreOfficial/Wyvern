#pragma once

#include <wv/Types.h>
#include <wv/Assets/Texture.h>

namespace wv
{
	
	struct RenderTargetDesc
	{
		int width = 0;
		int height = 0;
		
		wv::TextureDesc* textureDescs;
		int numTextures;
	};

	class RenderTarget
	{
	public:
		wv::Handle fbHandle = 0;
		wv::Handle rbHandle = 0;

		/// TODOM: support multiple textures?
		wv::Texture** textures = 0;
		int numTextures = 0;

		int width = 0;
		int height = 0;
	};
}