#pragma once

#include <cm/Framework/Shader.h>

#include <string>
#include <map>

#include <cm/Framework/Buffer.h>
#include <cm/Framework/Shader.h>

namespace wv
{
	class cShader
	{
	public:
		 cShader();
		~cShader();

		cm::Shader::hShaderProgram shader_program_handle;

		void createUniformBlock();
		void uniformBlockBegin();
		void uniformBlockEnd();

		void uniformBlockBuffer( const std::string& _uniform, void* _data, size_t _size );

		int getUniformLocation( std::string _uniform );

	private:
		std::map<std::string, cm::Shader::sUniform> m_uniforms;

		cm::Shader::sUniformBlock m_uniform_block;
		cm::sBuffer m_uniform_buffer;

		unsigned char* m_uniform_buffer_data = nullptr;
		int m_buffer_slot = 0;
		

	};

}