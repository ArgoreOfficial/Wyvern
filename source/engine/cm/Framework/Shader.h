#pragma once

/*
 *
 * Shader.h
 * 
 * Type definitions for shader handles
 * 
 */

#include <string>

namespace cm
{
	namespace Shader
	{
		typedef unsigned int hShader;
		typedef unsigned int hShaderProgram;

		enum eShaderUniformType
		{
			ShaderUniformType_Float,
			ShaderUniformType_Int,
			ShaderUniformType_Double,
			ShaderUniformType_Bool,
			ShaderUniformType_Vec2,
			ShaderUniformType_Vec3,
			ShaderUniformType_Vec4,
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
		};
	}
}
