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
	struct sProgram;

	enum eShaderProgramType
	{
		WV_SHADER_TYPE_VERTEX,
		WV_SHADER_TYPE_FRAGMENT
	};

	struct sShaderProgramSource
	{
		Memory* data;
	};

	struct ShaderModuleDesc
	{
		eShaderProgramType type;
		sShaderProgramSource source;
	};

	struct sProgram
	{
		Handle handle;

		eShaderProgramType type;
		sShaderProgramSource source;

		std::vector<GPUBufferID> shaderBuffers;
		std::vector<sUniform> textureUniforms;

		void* pPlatformData;
	};
	
}