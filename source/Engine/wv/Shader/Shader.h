#pragma once

#include <wv/Types.h>
#include <string>

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
		std::string path;
	};

	struct ShaderDesc
	{
		ShaderSource* shaders;
		int numShaders;
	};
}