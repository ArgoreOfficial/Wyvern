#pragma once

#include <wv/Types.h>

namespace wv
{
	class Shader;

	struct ShaderProgramDesc
	{
		wv::Handle* shaders;
		int numShaders;
	};

	class ShaderProgram
	{
	public:
		wv::Handle handle;
	};
}