#include "cRenderer.h"

#include "../Window/cWindow.h"

#include <stdio.h>

lo::cRenderer::cRenderer()
{

}

lo::cRenderer::~cRenderer()
{

}

bool lo::cRenderer::create( cWindow& _window )
{
	if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
	{
		fprintf( stderr, "Failed to initialize GLAD\n" );
		return false;
	}
	glViewport( 0, 0, _window.getWidth(), _window.getHeight() );

	createDefaultShader();

	printf( "Lindorm Renderer Intialized\n" );
	return true;
}

void lo::cRenderer::clear( unsigned char _color )
{
	float r = ( _color & 0xFF000000 ) / 256.0f;
	float g = ( _color & 0x00FF0000 ) / 256.0f;
	float b = ( _color & 0x0000FF00 ) / 256.0f;
	float a = ( _color & 0x000000FF ) / 256.0f;

	glClearColor( r, g, b, a );
	glClear( GL_COLOR_BUFFER_BIT );
}

lo::cPipelineState lo::cRenderer::createPipelineState( sPipelineStateDesc* _desc )
{
	// vertex array object
	unsigned int vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	// create vertex buffer object
	unsigned int vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );

	// buffer data
	glBufferData( GL_ARRAY_BUFFER, _desc->vertexBufferDesc->size, _desc->vertexBufferDesc->data, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*)0 );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	cPipelineState ps;
	ps.state = 0;

	ps.state |= ( _desc->program  << PIPELINE_STATE_BIT_SHADER );
	ps.state |= ( _desc->hasAlpha << PIPELINE_STATE_BIT_ALPHA );
	ps.state |= ( _desc->cullFace << PIPELINE_STATE_BIT_CULLMODE );

	return ps;
}

void lo::cRenderer::execCmd( sCmd _cmd )
{
	switch ( _cmd.type )
	{
		case LO_CMD_TYPE_CLEAR: 
			clear( _cmd.idata); 
			break;

		case LO_CMD_TYPE_BIND_SHADER: 
			glUseProgram( _cmd.idata ); 
			break;

		case LO_CMD_TYPE_BIND_VERTEX_ARRAY_OBJECT: 
			glBindVertexArray( _cmd.idata ); 
			break;

		case LO_CMD_TYPE_BIND_DRAW: 
			glDrawArrays( GL_TRIANGLES, 0, 3 ); 
			break; /// TODO:

		case LO_CMD_TYPE_BIND_SWAP: 
			break;
	}
}

void lo::cRenderer::createDefaultShader( void )
{
	int  success;
	char infoLog[ 512 ];

	unsigned int vertex_shader;
	vertex_shader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertex_shader, 1, &m_vertex_shader_default_source, NULL );
	glCompileShader( vertex_shader );

	glGetShaderiv( vertex_shader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( vertex_shader, 512, NULL, infoLog );
		printf( "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s \n", infoLog );
	}

	unsigned int fragment_shader;
	fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragment_shader, 1, &m_fragment_shader_default_source, NULL );
	glCompileShader( fragment_shader );

	glGetShaderiv( vertex_shader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( vertex_shader, 512, NULL, infoLog );
		printf( "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s \n", infoLog );
	}

	m_shader_default = glCreateProgram();

	glAttachShader( m_shader_default, vertex_shader );
	glAttachShader( m_shader_default, fragment_shader );
	glLinkProgram( m_shader_default );

	glGetProgramiv( m_shader_default, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( m_shader_default, 512, NULL, infoLog );
		printf( "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n %s \n", infoLog );
	}

	glDeleteShader( vertex_shader );
	glDeleteShader( fragment_shader );
}
