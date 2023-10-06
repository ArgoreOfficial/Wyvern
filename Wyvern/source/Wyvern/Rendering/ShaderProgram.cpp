#include "ShaderProgram.h"

ShaderProgram::ShaderProgram( ShaderSource source )
{ 
	_renderID = glCreateProgram();

	unsigned int vertShader = CompileShader( GL_VERTEX_SHADER, source.vertexSource.c_str() );
	unsigned int fragShader = CompileShader( GL_FRAGMENT_SHADER, source.fragmentSource.c_str() );

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

unsigned int ShaderProgram::CompileShader( unsigned int type, const std::string& _source )
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
		std::cout << "ERROR\n" << infoLog << std::endl;
	};

	return id;
}
