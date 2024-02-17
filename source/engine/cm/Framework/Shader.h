#pragma once

/*
 *
 * Shader.h
 * 
 * Type definitions for shader objects
 * 
 */

#include <string>
#include <vector>

namespace cm
{
	namespace Shader
	{
		typedef unsigned int hShader;
		typedef unsigned int hShaderProgram;

		enum eShaderUniformType
		{
			ShaderUniformType_Other,
			ShaderUniformType_Sampler1D,
			ShaderUniformType_Sampler2D,
			ShaderUniformType_Sampler3D,
			ShaderUniformType_SamplerCube,
		};

		enum eShaderType
		{
			ShaderType_Vertex, 
			ShaderType_Fragment,
			ShaderType_Geometry,
			ShaderType_Compute /* not implemented */
		};

		struct sShader
		{
			const hShader handle;
			const eShaderType type;
		};

		struct sShaderUniform
		{
			std::string name;
			int location;
			int type;
			size_t size;
		};

		struct sShaderUniformBlock
		{
			std::string name;
			int index;
			int size;
			std::vector<int> uniforms;
		};
	}
}
