#include "GraphicsDevice.h"

#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>

#include <glad/glad.h>
#include <stdio.h>

#include <sstream>
#include <fstream>

#include <vector>

wv::GraphicsDevice::GraphicsDevice( GraphicsDeviceDesc* _desc )
{
	/// TODO: make configurable
#ifdef EMSCRIPTEN
	int initRes = gladLoadGLES2Loader( _desc->loadProc );
	_desc->graphicsApi = WV_GRAPHICS_API_OPENGL_ES; // only option
#else
	int initRes = gladLoadGL();
#endif
	
	if( !initRes )
	{
		fprintf( stderr, "Failed to initialize Graphics Device\n" );
		return;
	}
	
	printf( "Intialized Graphics Device\n" );

	switch ( _desc->graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:    printf( "  OpenGL %s\n", glGetString( GL_VERSION ) ); break;
	case WV_GRAPHICS_API_OPENGL_ES: printf( "  %s\n", glGetString( GL_VERSION ) ); break;
	}
}

void wv::GraphicsDevice::setRenderTarget( DummyRenderTarget* _target )
{
	glBindFramebuffer( GL_FRAMEBUFFER, _target->framebuffer ); /// TEMPORARY
	glViewport( 0, 0, _target->width, _target->height );
}

void wv::GraphicsDevice::clearRenderTarget( const float _color[ 4 ] )
{
	glClearColor( _color[ 0 ], _color[ 1 ], _color[ 2 ], _color[ 3 ] );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

wv::Pipeline* wv::GraphicsDevice::createPipeline( PipelineDesc* _desc )
{
	wv::Pipeline* pipeline = new wv::Pipeline();

	switch ( _desc->type )
	{
	case WV_PIPELINE_GRAPHICS: 
	{
		std::vector<wv::Handle> shaders;

		// create shaders
		for ( int i = 0; i < _desc->numShaders; i++ )
			shaders.push_back( createShader( &_desc->shaders[ i ] ) );

		// create program
		{
			wv::ShaderProgramDesc programDesc;
			programDesc.shaders = shaders.data();
			programDesc.numShaders = _desc->numShaders;
			pipeline->program = createProgram( &programDesc );
		}

		// clean up shaders
		for ( int i = 0; i < _desc->numShaders; i++ )
			glDeleteShader( shaders[ i ] );

	} break;
	}

	return pipeline;
}

void wv::GraphicsDevice::destroyPipeline( Pipeline** _pipeline )
{
	glDeleteProgram( ( *_pipeline )->program );

	delete *_pipeline;
	*_pipeline = nullptr;
}

void wv::GraphicsDevice::setActivePipeline( Pipeline* _pipeline )
{
	glUseProgram( _pipeline->program );
}

wv::Primitive* wv::GraphicsDevice::createPrimitive( PrimitiveDesc* _desc )
{
	Primitive* primitive = new Primitive();
	{
		glGenVertexArrays( 1, &primitive->handle );
		glBindVertexArray( primitive->handle );

		// create vertex buffer object
		unsigned int vbo;
		glGenBuffers( 1, &vbo );
		glBindBuffer( GL_ARRAY_BUFFER, vbo );

		GLenum usage = GL_NONE;
		switch ( _desc->type )
		{
		case wv::WV_PRIMITIVE_TYPE_STATIC: usage = GL_STATIC_DRAW; break;
		}

		// buffer data
		glBufferData( GL_ARRAY_BUFFER, _desc->vertexBufferSize, _desc->vertexBuffer, usage );

		for ( int i = 0; i < _desc->layout->numElements; i++ )
		{
			InputLayoutElement& element = _desc->layout->elements[ i ];

			GLenum type = GL_FLOAT;
			switch ( _desc->layout->elements[ i ].type )
			{
			case WV_BYTE:           type = GL_BYTE; break;
			case WV_UNSIGNED_BYTE:  type = GL_UNSIGNED_BYTE; break;
			case WV_SHORT:          type = GL_SHORT; break;
			case WV_UNSIGNED_SHORT: type = GL_UNSIGNED_SHORT; break;
			case WV_INT:            type = GL_INT; break;
			case WV_UNSIGNED_INT:   type = GL_UNSIGNED_INT; break;
			case WV_FLOAT:          type = GL_FLOAT; break;
		#ifndef EMSCRIPTEN
			case WV_DOUBLE:         type = GL_DOUBLE; break;
		#endif
			}

			glVertexAttribPointer( i, element.num, type, element.normalized, element.stride, (void*)0 );
			glEnableVertexAttribArray( i );
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );

		primitive->count = _desc->numVertices; /// TODO: allow indices
	}

    return primitive;
}

void wv::GraphicsDevice::draw( Primitive* _primitive )
{
	glBindVertexArray( _primitive->handle );

	/// TODO: change GL_TRIANGLES
	glDrawArrays( GL_TRIANGLES, 0, _primitive->count );
}

wv::Handle wv::GraphicsDevice::createShader( ShaderSource* _desc )
{
	int  success;
	char infoLog[ 512 ];

	std::ifstream fs( _desc->filePath );
	std::stringstream buffer;
	buffer << fs.rdbuf();
	std::string source_str = buffer.str();

	// GLSL specification (chapter 3.3) requires that #version be the first thing in a shader source
	// therefore #if GL_ES cannot be used in the shader itself
#ifdef EMSCRIPTEN
	source_str = "#version 300 es\n" + source_str;
#else
	source_str = "#version 460 core\n" + source_str;
#endif
	const char* source = source_str.c_str();

	GLenum type = GL_NONE;
	{
		switch ( _desc->type )
		{
		case wv::WV_SHADER_TYPE_FRAGMENT: type = GL_FRAGMENT_SHADER; break;
		case wv::WV_SHADER_TYPE_VERTEX: type = GL_VERTEX_SHADER; break;
		}
	}

	wv::Handle shader;
	shader = glCreateShader( type );
	glShaderSource( shader, 1, &source, NULL);
	glCompileShader( shader );

	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( shader, 512, NULL, infoLog );
		printf( "Shader compilation failed in '%s'\n %s \n", _desc->filePath, infoLog );
	}

    return shader;
}

wv::Handle wv::GraphicsDevice::createProgram( ShaderProgramDesc* _desc )
{
	int  success;
	char infoLog[ 512 ];

	wv::Handle program;
	program = glCreateProgram();

	for ( int i = 0; i < _desc->numShaders; i++ )
		glAttachShader( program, _desc->shaders[i] );
	
	glLinkProgram( program );

	glGetProgramiv( program, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( program, 512, NULL, infoLog );
		printf( "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n %s \n", infoLog );
	}

	return program;
}
