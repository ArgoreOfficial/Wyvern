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
		 cShader( std::string _name, std::string _path );
		~cShader();

		void destroy();

		void createUniformBlock();

		void ubBegin     ( void );
		void ubEnd       ( void );
		void ubBufferData( const std::string& _uniform, void* _data, size_t _size );

		int getUniformLocation( std::string _uniform );

		cm::Shader::hShaderProgram shader_program_handle;

		const std::string name;
		const std::string path;

	private:
		std::map<std::string, cm::Shader::sUniform> m_uniforms;
		std::map<std::string, bool> m_used_uniforms;

		cm::Shader::sUniformBlock m_uniform_block;
		cm::sBuffer m_uniform_buffer;

		unsigned char* m_uniform_buffer_data = nullptr;
		int m_buffer_slot = 0;
	};

}