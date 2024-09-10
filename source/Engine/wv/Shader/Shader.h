#pragma once

#include <wv/Resource/Resource.h>
#include <wv/Graphics/Uniform.h>
#include <wv/Graphics/GPUBuffer.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct Memory;

	enum eShaderProgramType
	{
		WV_SHADER_TYPE_VERTEX,
		WV_SHADER_TYPE_FRAGMENT
	};

	struct sShaderProgramSource
	{
		Memory* data;
	};

	struct sShaderProgram
	{
		Handle handle;

		eShaderProgramType type;
		sShaderProgramSource source;

		std::vector<sGPUBuffer*> shaderBuffers;
		std::vector<sUniform> textureUniforms;

		sPlatformData pPlatformData;
	};

}