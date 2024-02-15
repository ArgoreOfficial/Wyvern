#pragma once

#include <cm/Framework/Shader.h>

#include <string>
#include <map>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>
#include <cm/Framework/Buffer.h>

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

		template<class T>
		void uniformBlockBuffer( const std::string& _uniform, T* _data, size_t _size );

		int getUniformLocation( std::string _uniform );

	private:
		std::map<std::string, cm::Shader::sShaderUniform> m_uniforms;
		cm::sBuffer m_uniform_buffer;
		unsigned char* m_uniform_buffer_data = nullptr;
		int m_buffer_offset = 0;
		int m_ubo_size = 0;
		int m_buffer_slot = 0;

	};

	template<class T>
	inline void cShader::uniformBlockBuffer( const std::string& _uniform, T* _data, size_t _size )
	{
		memcpy( &m_uniform_buffer_data[ m_buffer_offset ], _data, _size );
		m_buffer_offset += _size;
	}
}