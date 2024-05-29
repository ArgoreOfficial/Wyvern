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

	struct ShaderDesc
	{
		ShaderSource* shaders;
		int numShaders;
	};
}