#pragma once

#include <wv/resource/resource.h>
#include <wv/graphics/uniform.h>
#include <wv/graphics/gpu_buffer.h>

#include <wv/graphics/types.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct Memory;
	struct Program;

	enum ShaderProgramType
	{
		WV_SHADER_TYPE_VERTEX,
		WV_SHADER_TYPE_FRAGMENT
	};

	struct ShaderProgramSource
	{
		Memory* data;
	};

	struct ShaderModuleDesc
	{
		ShaderProgramType type;
		ShaderProgramSource source;
	};

	struct Program
	{
		Handle handle;

		ShaderProgramType type;
		ShaderProgramSource source;

		std::vector<GPUBufferID> shaderBuffers;
		std::vector<Uniform> textureUniforms;

		void* pPlatformData;
	};
	
}