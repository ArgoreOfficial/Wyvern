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
	struct sShaderProgram;

	enum eShaderProgramType
	{
		WV_SHADER_TYPE_VERTEX,
		WV_SHADER_TYPE_FRAGMENT
	};

	struct sShaderProgramSource
	{
		Memory* data;
	};

	struct sShaderProgramDesc
	{
		eShaderProgramType type;
		sShaderProgramSource source;
	};

	struct sShaderProgram
	{
		Handle handle;

		eShaderProgramType type;
		sShaderProgramSource source;

		std::vector<cGPUBuffer*> shaderBuffers;
		std::vector<sUniform> textureUniforms;

		void* pPlatformData;
	};
	
	WV_OPAQUE_HANDLE( ShaderProgramID );
}