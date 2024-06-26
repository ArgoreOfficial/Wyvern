#include "GraphicsDevice.h"

#include <wv/Assets/Texture.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Debug/Print.h>
#include <wv/Decl.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Primitive/Primitive.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <glad/glad.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

wv::iGraphicsDevice::iGraphicsDevice( GraphicsDeviceDesc* _desc )
{
	/// TODO: make configurable

	m_graphicsApi = _desc->graphicsApi;
	m_graphicsApiVersion = _desc->graphicsApiVersion;

	int initRes = 0;
	switch ( _desc->graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:     initRes = gladLoadGLLoader( _desc->loadProc ); break;
	case WV_GRAPHICS_API_OPENGL_ES1: initRes = gladLoadGLES1Loader( _desc->loadProc ); break;
	case WV_GRAPHICS_API_OPENGL_ES2: initRes = gladLoadGLES2Loader( _desc->loadProc ); break;
	}

	if ( !initRes )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "Failed to initialize Graphics Device\n" );
		return;
	}

	Debug::Print( Debug::WV_PRINT_INFO, "Intialized Graphics Device\n" );
	Debug::Print( Debug::WV_PRINT_INFO, "  %s\n", glGetString( GL_VERSION ) );

	/// TEMPORARY---
	/// TODO: add to pipeline configuration
	//glEnable( GL_MULTISAMPLE );
	//glEnable( GL_BLEND );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );
	glEnable( GL_CULL_FACE );
	/// ---TEMPORARY
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::terminate()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::onResize( int _width, int _height )
{
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::setViewport( int _width, int _height )
{
	glViewport( 0, 0, _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTarget* wv::iGraphicsDevice::createRenderTarget( RenderTargetDesc* _desc )
{
	RenderTarget* target = new RenderTarget();
	
	glGenFramebuffers( 1, &target->fbHandle );
	glBindFramebuffer( GL_FRAMEBUFFER, target->fbHandle );

	target->numTextures = _desc->numTextures;
	GLenum* drawBuffers = new GLenum[ _desc->numTextures ];
	target->textures = new Texture * [ _desc->numTextures ];
	for ( int i = 0; i < _desc->numTextures; i++ )
	{
		_desc->textureDescs[ i ].width = _desc->width;
		_desc->textureDescs[ i ].height = _desc->height;

		target->textures[ i ] = createTexture( &_desc->textureDescs[ i ] );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, target->textures[ i ]->handle, 0 );

		drawBuffers[ i ] = GL_COLOR_ATTACHMENT0 + i;
	}

	glGenRenderbuffers( 1, &target->rbHandle );
	glBindRenderbuffer( GL_RENDERBUFFER, target->rbHandle );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _desc->width, _desc->height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->rbHandle );
	
	glDrawBuffers( _desc->numTextures, drawBuffers );
	delete[] drawBuffers;
#ifndef WV_PACKAGE
	int errcode = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( errcode != GL_FRAMEBUFFER_COMPLETE )
	{
		const char* err = "";
		switch ( errcode )
		{
		case GL_FRAMEBUFFER_UNDEFINED:                     err = "GL_FRAMEBUFFER_UNDEFINED"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         err = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: err = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        err = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        err = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
		case GL_FRAMEBUFFER_UNSUPPORTED:                   err = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        err = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      err = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
		case GL_INVALID_ENUM:                              err = "GL_INVALID_ENUM "; break;
		case GL_INVALID_OPERATION:                         err = "GL_INVALID_OPERATION "; break;
		}

		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to create RenderTarget\n" );
		Debug::Print( Debug::WV_PRINT_ERROR, "  %s\n", err );

		destroyRenderTarget( &target );
		delete target;
		return nullptr;
	}
#endif
	target->width = _desc->width;
	target->height = _desc->height;

	glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return target;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::destroyRenderTarget( RenderTarget** _renderTarget )
{
	RenderTarget* rt = *_renderTarget;

	glDeleteFramebuffers( 1, &rt->fbHandle );
	glDeleteRenderbuffers( 1, &rt->rbHandle );

	for ( int i = 0; i < rt->numTextures; i++ )
		glDeleteTextures( 1, &rt->textures[ i ]->handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::setRenderTarget( RenderTarget* _target )
{
	unsigned int handle = 0;
	if ( _target )
		handle = _target->fbHandle;

	glBindFramebuffer( GL_FRAMEBUFFER, handle );
	if ( _target )
		glViewport( 0, 0, _target->width, _target->height );
	else
		glViewport( 0, 0, 640, 480 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::setClearColor( const wv::cColor& _color )
{
	glClearColor( _color.r, _color.g, _color.b, _color.a );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::clearRenderTarget( bool _color, bool _depth )
{
	glClear( (GL_COLOR_BUFFER_BIT * _color) | (GL_DEPTH_BUFFER_BIT * _depth) );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cShader* wv::iGraphicsDevice::createShader( eShaderType _type, const std::string& _name )
{
	
	cShader* shader = new cShader( this, _type, _name );

	GLenum type = GL_NONE;
	{
		switch ( _type )
		{
		case wv::WV_SHADER_TYPE_FRAGMENT: type = GL_FRAGMENT_SHADER; break;
		case wv::WV_SHADER_TYPE_VERTEX: type = GL_VERTEX_SHADER; break;
		}
	}

	wv::Handle handle = glCreateShader( type );
#ifdef WV_DEBUG
	assertGLError( "Failed to create shader\n" );
#endif
	shader->setHandle( handle );
	
	return shader;
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::destroyShader( cShader* _shader )
{
	glDeleteShader( _shader->getHandle() );
	_shader->setHandle( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::compileShader( cShader* _shader )
{
	if ( !_shader )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile null shader\n" );
		return;
	}
	
	if ( _shader->m_source == "" )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile shader with null source\n" );
		return;
	}

	// GLSL specification (chapter 3.3) requires that #version be the first thing in a shader source
	// therefore '#if GL_ES' cannot be used in the shader itself
	/// TODO: move /// This will keep adding if compileShader is called more than once on the same shader
#ifdef EMSCRIPTEN
	_shader->m_source = "#version 300 es\n" + _shader->m_source;
#else
	_shader->m_source = "#version 460 core\n" + _shader->m_source;
#endif
	const char* sourcePtr = _shader->m_source.c_str();

	wv::Handle shaderHandle = _shader->getHandle();
	glShaderSource( shaderHandle, 1, &sourcePtr, NULL);
#ifdef WV_DEBUG
	assertGLError( "Failed to add shader source to program\n" );
#endif

	glCompileShader( shaderHandle );
	
	int  success;
	char infoLog[ 512 ];
	glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( shaderHandle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to compile shader '%s'\n %s \n", _shader->m_name.c_str(), infoLog );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cShaderProgram* wv::iGraphicsDevice::createProgram()
{
	wv::cShaderProgram* program = new cShaderProgram( "Program" ); /// move
	wv::Handle programHandle = glCreateProgram();
#ifdef WV_DEBUG
	assertGLError( "Failed to create program\n" );
#endif
	program->setHandle( programHandle );

	return program;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::destroyProgram( cShaderProgram* _program )
{
	glDeleteProgram( _program->getHandle() );
	_program->setHandle( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::linkProgram( cShaderProgram* _program, std::vector<UniformBlockDesc> _uniformBlocks, std::vector<Uniform> _textureUniforms )
{
	if ( !_program )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot link null program\n" );
		return;
	}

	wv::Handle programHandle = _program->getHandle();
	if ( programHandle == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot link program with null handle\n" );
		return;
	}

	std::vector<cShader*> shaders = _program->getShaders();

	for ( int i = 0; i < shaders.size(); i++ )
	{
		glAttachShader( programHandle, shaders[ i ]->getHandle() );
	#ifdef WV_DEBUG
		assertGLError( "attach\n" );
	#endif
	}
	glLinkProgram( programHandle );
#ifdef WV_DEBUG
	assertGLError( "link\n" );
#endif

	int  success;
	char infoLog[ 512 ];
	glGetProgramiv( programHandle, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( programHandle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to link program\n %s \n", infoLog );
	}

	// clean up shaders
	//for ( int i = 0; i < (int)_desc->shaders.size(); i++ )
	//	glDeleteShader( shaders[ i ] );

	/// TODO: move? I don't like having these vectors in linkProgram()
	for ( int i = 0; i < (int)_uniformBlocks.size(); i++ )
	{
		UniformBlock block = createUniformBlock( _program, &_uniformBlocks[ i ] );
		_program->addUniformBlock( _uniformBlocks[ i ].name, block );
	}

	glUseProgram( programHandle );

	// required for OpenGL < 4.2
	// could probably be skipped for OpenGL >= 4.2 but would require layout(binding=i) in the shader source
	for ( int i = 0; i < (int)_textureUniforms.size(); i++ )
	{
		unsigned int loc = glGetUniformLocation( programHandle, _textureUniforms[ i ].name.c_str() );
		glUniform1i( loc, _textureUniforms[ i ].index );
	#ifdef WV_DEBUG
		assertGLError( "Failed to bind texture loc\n" );
	#endif
	}

	glUseProgram( 0 );

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::useProgram( cShaderProgram* _program )
{
	glUseProgram( _program ? _program->getHandle() : 0 );
	m_activeProgram = _program;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Mesh* wv::iGraphicsDevice::createMesh( MeshDesc* _desc )
{
	Mesh* mesh = new Mesh();
	/// TODO: remove?
	return mesh;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::destroyMesh( Mesh** _mesh )
{
	Debug::Print( Debug::WV_PRINT_DEBUG, "Destroyed mesh\n" );

	Mesh* mesh = *_mesh;
	for ( int i = 0; i < mesh->primitives.size(); i++ )
		destroyPrimitive( &mesh->primitives[ i ] );
	mesh->primitives.clear();
	*_mesh = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Primitive* wv::iGraphicsDevice::createPrimitive( PrimitiveDesc* _desc, Mesh* _mesh )
{
	Primitive* primitive = new Primitive();
	glGenVertexArrays( 1, &primitive->vaoHandle );
	glBindVertexArray( primitive->vaoHandle );
	
	// create vertex buffer object
	glGenBuffers( 1, &primitive->vboHandle );
	glBindBuffer( GL_ARRAY_BUFFER, primitive->vboHandle );

	GLenum usage = GL_NONE;
	switch ( _desc->type ) /// TODO: more primitive types
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
		case WV_BYTE:           type = GL_BYTE;           break;
		case WV_UNSIGNED_BYTE:  type = GL_UNSIGNED_BYTE;  break;
		case WV_SHORT:          type = GL_SHORT;          break;
		case WV_UNSIGNED_SHORT: type = GL_UNSIGNED_SHORT; break;
		case WV_INT:            type = GL_INT;            break;
		case WV_UNSIGNED_INT:   type = GL_UNSIGNED_INT;   break;
		case WV_FLOAT:          type = GL_FLOAT;          break;
		#ifndef EMSCRIPTEN // WebGL does not support GL_DOUBLE
		case WV_DOUBLE:         type = GL_DOUBLE; break;
		#endif
		}

		glVertexAttribPointer( i, element.num, type, element.normalized, stride, VPTRi32( offset ) );
		glEnableVertexAttribArray( i );

		offset += element.size;
	}

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	primitive->numVertices = _desc->numVertices; 
	primitive->stride = stride;
	
	_mesh->primitives.push_back( primitive );

	return primitive;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::destroyPrimitive( Primitive** _primitive )
{
	Primitive* pr = *_primitive;
	glDeleteBuffers( 1, &pr->eboHandle );
	glDeleteBuffers( 1, &pr->vboHandle );
	glDeleteVertexArrays( 1, &pr->vaoHandle );
	delete pr;
	*_primitive = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Texture* wv::iGraphicsDevice::createTexture( TextureDesc* _desc )
{
	Texture* texture = new Texture();

	GLenum internalFormat = GL_R8;
	GLenum format = GL_RED;

	unsigned char* data = nullptr;
	if ( _desc->memory )
	{
		data = _desc->memory->data;
		_desc->width = _desc->memory->width;
		_desc->height = _desc->memory->height;
		_desc->channels = static_cast<wv::TextureChannels>( _desc->memory->numChannels );
	}

	switch ( _desc->channels )
	{
	case wv::WV_TEXTURE_CHANNELS_R:
		format = GL_RED;
		switch ( _desc->format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE:  internalFormat = GL_R8;  break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_R32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:   internalFormat = GL_R32I; format = GL_RED_INTEGER; break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RG:
		format = GL_RG;
		switch ( _desc->format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE:  internalFormat = GL_RG8;    break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_RG32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:   internalFormat = GL_RG32I; format = GL_RG_INTEGER; break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RGB:
		format = GL_RGB;
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // 3 (channels) is not divisible by 4. Set pixel alignment to 1
		switch ( _desc->format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE:  internalFormat = GL_RGB8;   break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_RGB32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:   internalFormat = GL_RGB32I; format = GL_RGB_INTEGER; break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RGBA:
		format = GL_RGBA;
		switch ( _desc->format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE:  internalFormat = GL_RGBA8;   break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_RGBA32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:   internalFormat = GL_RGBA32I; format = GL_RGBA_INTEGER; break;
		}
		break;
	}

	glGenTextures( 1, &texture->handle );
#ifdef WV_DEBUG
	assertGLError( "Failed to gen texture\n" );
#endif
	
	glBindTexture( GL_TEXTURE_2D, texture->handle );
#ifdef WV_DEBUG
	assertGLError( "Failed to bind texture\n" );
#endif
	
	GLenum filter = GL_NEAREST;
	switch ( _desc->filtering )
	{
	case WV_TEXTURE_FILTER_NEAREST: filter = GL_NEAREST; break;
	case WV_TEXTURE_FILTER_LINEAR:  filter = GL_LINEAR; break;
	}
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	GLenum type = GL_UNSIGNED_BYTE;
	switch ( _desc->format )
	{
	case wv::WV_TEXTURE_FORMAT_FLOAT: type = GL_FLOAT; break;
	case wv::WV_TEXTURE_FORMAT_INT:   type = GL_INT; break;
	}

	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, _desc->width, _desc->height, 0, format, type, data );	
#ifdef WV_DEBUG
	assertGLError( "Failed to create Texture\n" );
#endif

	if( _desc->generateMipMaps )
		glGenerateMipmap( GL_TEXTURE_2D );

	texture->width  = _desc->width;
	texture->height = _desc->height;
	return texture;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::destroyTexture( Texture** _texture )
{
	Debug::Print( Debug::WV_PRINT_DEBUG, "Destroyed texture\n" );

	glDeleteTextures( 1, &( *_texture )->handle );
	delete *_texture;
	*_texture = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::bindTextureToSlot( Texture* _texture, unsigned int _slot )
{
	/// TODO: some cleaner way of checking version/supported features
	if ( m_graphicsApiVersion.major == 4 && m_graphicsApiVersion.minor >= 5 ) // if OpenGL 4.5 or higher
	{
		glBindTextureUnit( _slot, _texture->handle );
	}
	else 
	{
		glActiveTexture( GL_TEXTURE0 + _slot );
		glBindTexture( GL_TEXTURE_2D, _texture->handle );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::draw( Mesh* _mesh )
{
	if ( !_mesh )
		return;

	for ( size_t i = 0; i < _mesh->primitives.size(); i++ )
	{
		if ( _mesh->primitives[ i ]->material )
		{
			_mesh->primitives[ i ]->material->setAsActive( this );
			_mesh->primitives[ i ]->material->instanceCallback( _mesh );
		}

		drawPrimitive( _mesh->primitives[ i ] );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::iGraphicsDevice::getError( std::string* _out )
{
	bool hasError = false;

	GLenum err;
	while ( ( err = glGetError() ) != GL_NO_ERROR ) // is while needed here?
	{
		hasError = true;
		switch ( err )
		{
		case GL_INVALID_ENUM:      *_out = "GL_INVALID_ENUM";      break;
		case GL_INVALID_VALUE:     *_out = "GL_INVALID_VALUE";     break;
		case GL_INVALID_OPERATION: *_out = "GL_INVALID_OPERATION"; break;
		
		case GL_STACK_OVERFLOW:  *_out = "GL_STACK_OVERFLOW";    break;
		case GL_STACK_UNDERFLOW: *_out = "GL_STACK_UNDERFLOW";   break;
		case GL_OUT_OF_MEMORY:   *_out = "GL_OUT_OF_MEMORY";     break;
		case GL_CONTEXT_LOST:    *_out = "GL_OUT_OF_MEMORY";     break;

		case GL_INVALID_FRAMEBUFFER_OPERATION: *_out = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		}
	}

	return hasError;
}

void wv::iGraphicsDevice::drawPrimitive( Primitive* _primitive )
{
	glBindVertexArray( _primitive->vaoHandle );
	
	UniformBlockMap* uniformBlocks = m_activeProgram->getUniformBlockMap();
	for ( auto& block : *uniformBlocks )
	{
		glUniformBlockBinding( m_activeProgram->getHandle(), block.second.m_index, block.second.m_bindingIndex);
		glBindBuffer( GL_UNIFORM_BUFFER, block.second.m_bufferHandle );
		glBufferData( GL_UNIFORM_BUFFER, block.second.m_bufferSize, block.second.m_buffer, GL_DYNAMIC_DRAW );
	}

	/// TODO: change GL_TRIANGLES
	if ( _primitive->drawType == WV_PRIMITIVE_DRAW_TYPE_INDICES )
		glDrawElements( GL_TRIANGLES, _primitive->numIndices, GL_UNSIGNED_INT, 0 );
	else
	{ 
	#ifndef EMSCRIPTEN
		/// this does not work on WebGL
		glBindVertexBuffer( 0, _primitive->vboHandle, 0, _primitive->stride );
		glDrawArrays( GL_TRIANGLES, 0, _primitive->numVertices );
	#else
		Debug::Print( Debug::WV_PRINT_FATAL, "glBindVertexBuffer is not supported on WebGL. Index Buffer is required\n" );
	#endif
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::UniformBlock wv::iGraphicsDevice::createUniformBlock( cShaderProgram* _program, UniformBlockDesc* _desc )
{
	UniformBlock block;
	Handle programHandle = _program->getHandle();

	block.m_bindingIndex = m_numTotalUniformBlocks;
	
	block.m_index = glGetUniformBlockIndex( programHandle, _desc->name.c_str() );;
	glGetActiveUniformBlockiv( programHandle, block.m_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block.m_bufferSize );

	block.m_buffer = new char[ block.m_bufferSize ];
	memset( block.m_buffer, 0, block.m_bufferSize );
	
	glGenBuffers( 1, &block.m_bufferHandle );
	glBindBuffer( GL_UNIFORM_BUFFER, block.m_bufferHandle );
	glBufferData( GL_UNIFORM_BUFFER, block.m_bufferSize, 0, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	
	glBindBufferBase( GL_UNIFORM_BUFFER, block.m_bindingIndex, block.m_bufferHandle );

	std::vector<unsigned int> indices( _desc->uniforms.size() );
	std::vector<int> offsets( _desc->uniforms.size() );
	
	std::vector<const char*> uniformNames;
	for ( size_t i = 0; i < _desc->uniforms.size(); i++ )
		uniformNames.push_back( _desc->uniforms[ i ].name.c_str() ); // lifetime issues?
	
	glGetUniformIndices( programHandle, (GLsizei)_desc->uniforms.size(), uniformNames.data(), indices.data() );
	glGetActiveUniformsiv( programHandle, (GLsizei)_desc->uniforms.size(), indices.data(), GL_UNIFORM_OFFSET, offsets.data());

	for ( int o = 0; o < _desc->uniforms.size(); o++ )
	{
		Uniform u;
		u.index = indices[ o ];
		u.offset = offsets[ o ];
		u.name = _desc->uniforms[ o ].name;

		block.m_uniforms[ u.name ] = u;
	}

	m_numTotalUniformBlocks++;

	return block;
}
