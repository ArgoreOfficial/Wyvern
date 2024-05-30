#pragma once

#include <wv/Types.h>

namespace wv
{
	struct RenderTargetDesc
	{
		int width = 0;
		int height = 0;
		// format
	};

	class RenderTarget
	{
	public:
		wv::Handle fbHandle = 0;
		wv::Handle rbHandle = 0;

		/// TODOM: support multiple textures?
		wv::Handle texHandle = 0;
		
		int width = 0;
		int height = 0;
	};
}