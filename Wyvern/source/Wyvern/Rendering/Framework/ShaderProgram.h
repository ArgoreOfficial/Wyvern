#pragma once
#include "ShaderSource.h"
#include "GLHeader.h"
#include <iostream>
#include <unordered_map>

namespace WV
{
	class ShaderProgram
	{
	public:
		ShaderProgram( ShaderSource _source );
		~ShaderProgram();

		void glInit();

		void Bind() const { glUseProgram( _renderID ); }
		void Unbind() const { glUseProgram( 0 ); }
		void SetUniform1i( const std::string& name, int val );
		void SetUniform4f( const std::string& name, glm::vec4 vec );
		void SetUniformMat4( const std::string& name, glm::mat4 mat );

		int GetUniformLocation( const std::string& name );

		unsigned int getShaderProgram() { return( _renderID ); }
	
	private:
		unsigned int _renderID;
		std::unordered_map<std::string, int> _locationCache;
		ShaderSource m_source;
		
		unsigned int CompileShader( unsigned int type, const std::string& source, const std::string& _path );

	};
}