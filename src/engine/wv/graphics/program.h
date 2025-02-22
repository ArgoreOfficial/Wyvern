#pragma once

#include <wv/Resource/Resource.h>
#include <wv/Graphics/Uniform.h>
#include <wv/Graphics/GPUBuffer.h>

#include <vector>

#include <wv/Graphics/Types.h>

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