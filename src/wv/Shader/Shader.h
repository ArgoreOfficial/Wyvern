#pragma once

#include <wv/Types.h>

namespace wv
{
	enum ShaderType
	{
		WV_SHADER_TYPE_VERTEX,
		WV_SHADER_TYPE_FRAGMENT
	};

	struct ShaderSource
	{
		ShaderType type;
		const char* filePath;
	};

	class Shader
	{
	public:
		ShaderType type;
		wv::Handle handle;
	};
}