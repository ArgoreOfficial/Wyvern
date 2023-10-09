#include "ShaderProgram.h"
#include <Wyvern/Logging/Logging.h>
using namespace WV;

ShaderProgram::ShaderProgram( ShaderSource _source )
{
	_renderID = glCreateProgram();

	unsigned int vertShader = CompileShader( GL_VERTEX_SHADER, _source.m_vertexSource.c_str(), _source.m_path );
	unsigned int fragShader = CompileShader( GL_FRAGMENT_SHADER, _source.m_fragmentSource.c_str(), _source.m_path );

	glAttachShader( _renderID, vertShader );
	glAttachShader( _renderID, fragShader );
	glLinkProgram( _renderID );
	glValidateProgram( _renderID );

	glDeleteShader( vertShader );
	glDeleteShader( fragShader );

	// glUseProgram( 0 );
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram( _renderID );
}

void ShaderProgram::SetUniform1i( const std::string& name, int val )
{
	glUniform1i( GetUniformLocation( name ), val );
}

void ShaderProgram::SetUniform4f( const std::string& name, glm::vec4 vec )
{
	glUniform4f( GetUniformLocation( name ), vec.x, vec.y, vec.z, vec.w );
}

void ShaderProgram::SetUniformMat4( const std::string& name, glm::mat4 mat )
{
	glUniformMatrix4fv( GetUniformLocation( name ), 1, GL_FALSE, glm::value_ptr( mat ) );
}

int ShaderProgram::GetUniformLocation( const std::string& name )
{
	if ( _locationCache.find( name ) != _locationCache.end() ) { return _locationCache[ name ]; }

	int location = glGetUniformLocation( _renderID, name.c_str() );
	_locationCache[ name ] = location;
	return location;
}

unsigned int ShaderProgram::CompileShader( unsigned int type, const std::string& _source, const std::string& _path )
{
	unsigned int id = glCreateShader( type );
	const char* source = _source.c_str();

	glShaderSource( id, 1, &source, nullptr );
	glCompileShader( id );

	int success;
	char infoLog[ 512 ];
	glGetShaderiv( id, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( id, 512, NULL, infoLog );
		WVERROR( "Could not compile shader:\n    %s\n    %s", _path.c_str(), infoLog);
		return 0;
	};

	return id;
}
