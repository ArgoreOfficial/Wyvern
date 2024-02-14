#pragma once

#include <cm/Framework/Shader.h>

#include <string>
#include <map>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

namespace wv
{
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