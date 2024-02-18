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
#include <map>

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

		struct sUniform
		{
			std::string name;
			int location;
			int type;
			size_t size;
		};

		struct sUniformBlockVariable
		{
			std::string name;
			int location;
			int offset;
		};

		struct sUniformBlock
		{
			std::string name;
			int index;
			int size;
			std::map<std::string, sUniformBlockVariable> uniforms;
		};
	}
}
