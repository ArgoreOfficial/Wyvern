#pragma once

#include <cm/Framework/Shader.h>

#include <string>
#include <map>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

namespace wv
{
	enum eUniformDataType
	{
		UniformDataType_Float,
		UniformDataType_Vec3,
		UniformDataType_Vec4,
		UniformDataType_Texture,
		UniformDataType_Mat4,
	};

	struct sUniformData
	{
		std::string name;
		eUniformDataType type;
		union
		{
			float u_float;
			wv::cVector3f u_vec3;
			wv::cVector4f u_vec4;
			std::string u_tex_path;
			float* u_mat4;
		} data;
	};

	class cShader
	{
	public:
		 cShader();
		~cShader();

		cm::Shader::hShaderProgram shader_program_handle;

		int getUniformLocation( std::string _uniform );

		void setMatrix( std::string _uniform, float* _matrix_ptr );
		void setVec4f ( std::string _uniform, const wv::cVector4f& _vec );

	private:
		std::map<std::string, cm::Shader::sShaderUniform> m_uniforms;
	};
}