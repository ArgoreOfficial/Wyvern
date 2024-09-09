#pragma once

#include <wv/Resource/Resource.h>
#include <wv/Shader/UniformBlock.h>

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

		std::vector<cShaderBuffer*> shaderBuffers;
		std::vector<sUniform> textureUniforms;

		PlatformData pPlatformData;
	};

}