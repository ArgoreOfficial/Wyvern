#include "Wyvern/Renderer/Framework/cShaderProgram.h"

#include <Wyvern/Logging/cLogging.h>

#include <glad/gl.h>

using namespace wv;

///////////////////////////////////////////////////////////////////////////////////////

cShaderProgram::cShaderProgram( void )
{

}

///////////////////////////////////////////////////////////////////////////////////////

cShaderProgram::~cShaderProgram( void )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cShaderProgram::create( cShaderSource _vertexSource, cShaderSource _fragmentSource )
{

	m_handle = glCreateProgram();
	const char* vsource = _vertexSource.getSource();
	const char* fsource = _fragmentSource.getSource();

	// vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertexShader, 1, &vsource, NULL );
	glCompileShader( vertexShader );

	// fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragmentShader, 1, &fsource, NULL );
	glCompileShader( fragmentShader );

	glAttachShader( m_handle, vertexShader );
	glAttachShader( m_handle, fragmentShader );
	glLinkProgram( m_handle );

	glDeleteShader( vertexShader );
	glDeleteShader( fragmentShader );

}

///////////////////////////////////////////////////////////////////////////////////////

void cShaderProgram::use( void )
{
	glUseProgram( m_handle );
}
