#include "GraphicsDevice.h"

#include <wv/Assets/Texture.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>

#include <wv/RenderTarget/RenderTarget.h>

#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>

#include <glad/glad.h>
#include <stdio.h>

#include <sstream>
#include <fstream>

#include <vector>

wv::GraphicsDevice::GraphicsDevice( GraphicsDeviceDesc* _desc )
{
	/// TODO: make configurable

	int initRes = 0;
	switch ( _desc->graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:     initRes = gladLoadGLLoader( _desc->loadProc ); break;
	case WV_GRAPHICS_API_OPENGL_ES1: initRes = gladLoadGLES1Loader( _desc->loadProc ); break;
	case WV_GRAPHICS_API_OPENGL_ES2: initRes = gladLoadGLES2Loader( _desc->loadProc ); break;
	}

	if( !initRes )
	{
		fprintf( stderr, "Failed to initialize Graphics Device\n" );
		return;
	}
	
	printf( "Intialized Graphics Device\n" );
	printf( "  %s\n", glGetString( GL_VERSION ) ); 

	/// TEMPORARY---
	/// TODO: add to pipeline configuration
	//glEnable( GL_MULTISAMPLE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );
	glCullFace( GL_BACK );
	/// ---TEMPORARY
}

void wv::GraphicsDevice::terminate()
{
	
}

void wv::GraphicsDevice::onResize( int _width, int _height )
{
	// glViewport( 0, 0, _width, _height );
}

wv::RenderTarget* wv::GraphicsDevice::createRenderTarget( RenderTargetDesc* _desc )
{
	RenderTarget* target = new RenderTarget();

	glGenFramebuffers( 1, &target->fbHandle );
	glBindFramebuffer( GL_FRAMEBUFFER, target->fbHandle );

	glGenTextures( 1, &target->texHandle );
	glBindTexture( GL_TEXTURE_2D, target->texHandle );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, _desc->width, _desc->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 ); // empty texture
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glGenRenderbuffers( 1, &target->rbHandle );
	glBindRenderbuffer( GL_RENDERBUFFER, target->rbHandle );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _desc->width, _desc->height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->rbHandle );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target->texHandle, 0 );
	GLenum drawBuffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers( 1, drawBuffers );

	if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		printf( "Failed to create RenderTarget\n" );

		destroyRenderTarget( &target );
		delete target;
		return nullptr;
	}

	target->width = _desc->width;
	target->height = _desc->height;

	return target;
}

void wv::GraphicsDevice::destroyRenderTarget( RenderTarget** _renderTarget )
{
	RenderTarget* rt = *_renderTarget;
	glDeleteFramebuffers( 1, &rt->fbHandle );
	glDeleteRenderbuffers( 1, &rt->rbHandle );
	glDeleteTextures( 1, &rt->texHandle );
}

void wv::GraphicsDevice::setRenderTarget( RenderTarget* _target )
{
	unsigned int handle = 0;
	if ( _target )
		handle = _target->fbHandle;
	
	glBindFramebuffer( GL_FRAMEBUFFER, handle );
	if( _target )
		glViewport( 0, 0, _target->width, _target->height );
	else
		glViewport( 0, 0, 900, 600 );
}

void wv::GraphicsDevice::clearRenderTarget( const wv::Color& _color )
{
	glClearColor( _color.r, _color.g, _color.b, _color.a );
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
		for ( unsigned int i = 0; i < _desc->numShaders; i++ )
		{
			wv::Handle shader = createShader( &_desc->shaders[ i ] );
			if( shader != 0 )
				shaders.push_back( shader );
		}

		// create program
		{
			wv::ShaderProgramDesc programDesc;
			programDesc.shaders = shaders.data();
			programDesc.numShaders = _desc->numShaders;
			pipeline->program = createProgram( &programDesc );
		}

		// clean up shaders
		for ( unsigned int i = 0; i < _desc->numShaders; i++ )
			glDeleteShader( shaders[ i ] );


		for ( unsigned int i = 0; i < _desc->numUniformBlocks; i++ )
			createUniformBlock( pipeline, &_desc->uniformBlocks[ i ] );
		
		pipeline->instanceCallback = _desc->instanceCallback;
		pipeline->pipelineCallback = _desc->pipelineCallback;

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
	if ( m_activePipeline != _pipeline )
	{
		glUseProgram( _pipeline->program );
		m_activePipeline = _pipeline;
	}

	if ( m_activePipeline->pipelineCallback )
		m_activePipeline->pipelineCallback( m_activePipeline->uniformBlocks );
}

wv::Primitive* wv::GraphicsDevice::createPrimitive( PrimitiveDesc* _desc )
{
	Primitive* primitive = new Primitive();
	{
		glGenVertexArrays( 1, &primitive->vboHandle );
		glBindVertexArray( primitive->vboHandle );

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



		if ( _desc->indexBufferSize > 0 )
		{
			// create element buffer object
			primitive->drawType = WV_PRIMITIVE_DRAW_TYPE_INDICES;

			wv::Handle ebo;
			glGenBuffers( 1, &ebo );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, _desc->indexBufferSize, _desc->indexBuffer, usage );

			primitive->eboHandle = ebo;
			primitive->numIndices = _desc->numIndices;
		}
		else
		{
			primitive->drawType = WV_PRIMITIVE_DRAW_TYPE_VERTICES;
		}



		int offset = 0;
		int stride = 0;
		for ( unsigned int i = 0; i < _desc->layout->numElements; i++ )
		{
			InputLayoutElement& element = _desc->layout->elements[ i ];
			stride += element.size;
		}

		for ( unsigned int i = 0; i < _desc->layout->numElements; i++ )
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

			glVertexAttribPointer( i, element.num, type, element.normalized, stride, (void*)offset );
			glEnableVertexAttribArray( i );

			offset += element.size;
		}

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );

		primitive->numVertices = _desc->numVertices; /// TODO: allow indices
	}

    return primitive;
}

wv::Texture* wv::GraphicsDevice::createTexture( TextureDesc* _desc )
{
	/// TODO: move to asset loading  class thingymabob

	Texture* texture = new Texture();

	glGenTextures( 1, &texture->handle );
	glBindTexture( GL_TEXTURE_2D, texture->handle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	stbi_set_flip_vertically_on_load( 1 );
	int width, height, numChannels;
	unsigned char* data = stbi_load( _desc->filepath, &width, &height, &numChannels, 0 );
	if ( data )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	else
	{
		printf( "Failed to load texture\n" );
	}
	stbi_image_free( data );

	return texture;
}

void wv::GraphicsDevice::bindTextureToSlot( Texture* _texture, unsigned int _slot )
{
	glActiveTexture( GL_TEXTURE0 + _slot );
	glBindTexture( GL_TEXTURE_2D, _texture->handle );
}

void wv::GraphicsDevice::draw( Primitive* _primitive )
{
	glBindVertexArray( _primitive->vboHandle );

	if( m_activePipeline->instanceCallback )
		m_activePipeline->instanceCallback( m_activePipeline->uniformBlocks );

	for ( auto& block : m_activePipeline->uniformBlocks )
	{
		glUniformBlockBinding( m_activePipeline->program, block.second.m_index, block.second.m_bindingIndex );
		glBindBuffer( GL_UNIFORM_BUFFER, block.second.m_bufferHandle );
		glBufferData( GL_UNIFORM_BUFFER, block.second.m_bufferSize, block.second.m_buffer, GL_DYNAMIC_DRAW );
	}

	// m_activePipeline->mode

	/// TODO: change GL_TRIANGLES
	if ( _primitive->drawType == WV_PRIMITIVE_DRAW_TYPE_INDICES )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _primitive->eboHandle );
		glDrawElements( GL_TRIANGLES, _primitive->numIndices, GL_UNSIGNED_INT, 0 );
	}
	else
		glDrawArrays( GL_TRIANGLES, 0, _primitive->numVertices );
}

wv::Handle wv::GraphicsDevice::createShader( ShaderSource* _desc )
{
	int  success;
	char infoLog[ 512 ];


	/// TODO: change to proper asset loader
	std::ifstream fs( _desc->filePath );

	if ( !fs.is_open() )
	{
		printf( "Could not find shader source\n" );
		return 0;
	}

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

void wv::GraphicsDevice::createUniformBlock( Pipeline* _pipeline, UniformBlockDesc* _desc )
{
	UniformBlock block;

	block.m_bindingIndex = m_numTotalUniformBlocks;

	block.m_index = glGetUniformBlockIndex( _pipeline->program, _desc->name );
	glGetActiveUniformBlockiv( _pipeline->program, block.m_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block.m_bufferSize );

	block.m_buffer = new char[ block.m_bufferSize ];
	memset( block.m_buffer, 0, block.m_bufferSize );

	glGenBuffers( 1, &block.m_bufferHandle );
	glBindBuffer( GL_UNIFORM_BUFFER, block.m_bufferHandle );
	glBufferData( GL_UNIFORM_BUFFER, block.m_bufferSize, 0, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );

	glBindBufferBase( GL_UNIFORM_BUFFER, block.m_bindingIndex, block.m_bufferHandle );

	std::vector<unsigned int> indices( _desc->numUniforms );
	std::vector<int> offsets( _desc->numUniforms );

	glGetUniformIndices(   _pipeline->program, _desc->numUniforms, _desc->uniforms, indices.data() );
	glGetActiveUniformsiv( _pipeline->program, _desc->numUniforms, indices.data(), GL_UNIFORM_OFFSET, offsets.data() );

	for ( int o = 0; o < _desc->numUniforms; o++ )
	{
		Uniform u;
		u.index = indices[ o ];
		u.offset = offsets[ o ];
		u.name = _desc->uniforms[ o ]; // remove?

		block.m_uniforms[ u.name ] = u;
	}

	_pipeline->uniformBlocks[ _desc->name ] = block;
	m_numTotalUniformBlocks++;
}
