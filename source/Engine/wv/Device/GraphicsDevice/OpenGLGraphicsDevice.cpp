#include "OpenGLGraphicsDevice.h"

#include <wv/Assets/Texture.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Debug/Print.h>
#include <wv/Decl.h>
#include <wv/Memory/FileSystem.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Primitive/Primitive.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <wv/Device/DeviceContext.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif 

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

#ifdef WV_DEBUG
#define WV_ASSERT_GL( _func, ... ) \
if( _func != nullptr ) { \
	_func( __VA_ARGS__ ); \
} \
else { Debug::Print( Debug::WV_PRINT_FATAL, "Missing function '%s'\n", #_func ); }

#define WV_RETASSERT_GL( _ret, _func, ... ) if( _func != nullptr ) { _ret = _func( __VA_ARGS__ ); } else { Debug::Print( Debug::WV_PRINT_FATAL, "Missing function '%s'\n", #_func ); }

#define WV_ASSERT_ERR( _msg ) assertGLError( _msg )
#else
#define WV_ASSERT_GL( _func, ... ) _func( __VA_ARGS__ )
#define WV_RETASSERT_GL( _ret, _func, ... ) _ret = _func( __VA_ARGS__ )
#define WV_ASSERT_ERR( _msg ) 
#endif

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_OPENGL
static inline GLenum getGlBufferEnum( wv::eGPUBufferType _type )
{
	switch ( _type )
	{
	case wv::WV_BUFFER_TYPE_VERTEX: return GL_ARRAY_BUFFER;         break;
	case wv::WV_BUFFER_TYPE_INDEX:  return GL_ELEMENT_ARRAY_BUFFER; break;
	}

	return GL_NONE;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////

wv::cOpenGLGraphicsDevice::cOpenGLGraphicsDevice()
{
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cOpenGLGraphicsDevice::initialize( GraphicsDeviceDesc* _desc )
{
#ifdef WV_SUPPORT_OPENGL
	m_graphicsApi = _desc->pContext->getGraphicsAPI();
	
	Debug::Print( Debug::WV_PRINT_DEBUG, "Initializing Graphics Device...\n" );

	int initRes = 0;
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:     initRes = gladLoadGLLoader( _desc->loadProc ); break;
	case WV_GRAPHICS_API_OPENGL_ES1: initRes = gladLoadGLES1Loader( _desc->loadProc ); break;
	case WV_GRAPHICS_API_OPENGL_ES2: initRes = gladLoadGLES2Loader( _desc->loadProc ); break;
	}

	if ( !initRes )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "Failed to initialize Graphics Device\n" );
		return false;
	}

	Debug::Print( Debug::WV_PRINT_INFO, "Intialized Graphics Device\n" );
	Debug::Print( Debug::WV_PRINT_INFO, "  %s\n", glGetString( GL_VERSION ) );

	/// TODO: make configurable
	//glEnable( GL_MULTISAMPLE );
	//glEnable( GL_BLEND );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	WV_ASSERT_GL( glEnable, GL_DEPTH_TEST );
	WV_ASSERT_GL( glDepthFunc, GL_LESS );
	WV_ASSERT_GL( glEnable, GL_CULL_FACE );

	m_graphicsApiVersion.major = GLVersion.major;
	m_graphicsApiVersion.minor = GLVersion.minor;
	
	int numTextureUnits = 0;
	glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &numTextureUnits );

	m_boundTextureSlots.assign( numTextureUnits, 0 );
	return true;
#else
	return false;
#endif
}

void wv::cOpenGLGraphicsDevice::terminate()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::onResize( int _width, int _height )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setViewport( int _width, int _height )
{
#ifdef WV_SUPPORT_OPENGL
	WV_ASSERT_GL( glViewport, 0, 0, _width, _height );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTarget* wv::cOpenGLGraphicsDevice::createRenderTarget( RenderTargetDesc* _desc )
{
#ifdef WV_SUPPORT_OPENGL
	RenderTarget* target = new RenderTarget();
	
	WV_ASSERT_GL( glGenFramebuffers, 1, &target->fbHandle );
	WV_ASSERT_GL( glBindFramebuffer, GL_FRAMEBUFFER, target->fbHandle );
	
	target->numTextures = _desc->numTextures;
	GLenum* drawBuffers = new GLenum[ _desc->numTextures ];
	target->textures = new Texture * [ _desc->numTextures ];
	for ( int i = 0; i < _desc->numTextures; i++ )
	{
		_desc->textureDescs[ i ].width = _desc->width;
		_desc->textureDescs[ i ].height = _desc->height;

		std::string texname = "buffer_tex" + std::to_string( i );

		target->textures[ i ] = new Texture( texname );
		createTexture( target->textures[i], &_desc->textureDescs[i] );

		WV_ASSERT_GL( glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, target->textures[ i ]->getHandle(), 0);

		drawBuffers[ i ] = GL_COLOR_ATTACHMENT0 + i;
	}

	WV_ASSERT_GL( glGenRenderbuffers, 1, &target->rbHandle );
	WV_ASSERT_GL( glBindRenderbuffer, GL_RENDERBUFFER, target->rbHandle );
	WV_ASSERT_GL( glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _desc->width, _desc->height );
	WV_ASSERT_GL( glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->rbHandle );
	
	WV_ASSERT_GL( glDrawBuffers, _desc->numTextures, drawBuffers );
	delete[] drawBuffers;
#ifdef WV_DEBUG
	int errcode = 0; 
	WV_RETASSERT_GL( errcode, glCheckFramebufferStatus, GL_FRAMEBUFFER );
	
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

	WV_ASSERT_GL( glBindRenderbuffer, GL_RENDERBUFFER, 0 );
	WV_ASSERT_GL( glBindTexture, GL_TEXTURE_2D, 0 );
	WV_ASSERT_GL( glBindFramebuffer, GL_FRAMEBUFFER, 0 );

	return target;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyRenderTarget( RenderTarget** _renderTarget )
{
#ifdef WV_SUPPORT_OPENGL
	RenderTarget* rt = *_renderTarget;

	WV_ASSERT_GL( glDeleteFramebuffers, 1, &rt->fbHandle );
	WV_ASSERT_GL( glDeleteRenderbuffers, 1, &rt->rbHandle );

	for ( int i = 0; i < rt->numTextures; i++ )
	{
		destroyTexture( &rt->textures[ i ] );
		//wv::Handle handle = rt->textures[ i ]->getHandle();
		//WV_ASSERT_GL( glDeleteTextures, 1, &handle );
	}

	*_renderTarget = nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setRenderTarget( RenderTarget* _target )
{
#ifdef WV_SUPPORT_OPENGL
	if ( m_activeRenderTarget == _target )
		return;

	unsigned int handle = _target ? _target->fbHandle : 0;
	
	WV_ASSERT_GL( glBindFramebuffer, GL_FRAMEBUFFER, handle );
	if ( _target )
	{
		WV_ASSERT_GL( glViewport, 0, 0, _target->width, _target->height );
	}
	
	m_activeRenderTarget = _target;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setClearColor( const wv::cColor& _color )
{
#ifdef WV_SUPPORT_OPENGL
	WV_ASSERT_GL( glClearColor, _color.r, _color.g, _color.b, _color.a );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::clearRenderTarget( bool _color, bool _depth )
{
#ifdef WV_SUPPORT_OPENGL
	WV_ASSERT_GL( glClear, (GL_COLOR_BUFFER_BIT * _color) | (GL_DEPTH_BUFFER_BIT * _depth) );
#endif
}

wv::sShader* wv::cOpenGLGraphicsDevice::createShader( eShaderType _type, sShaderSource* _source )
{
	WV_RENDER_PRINT();

#ifdef WV_SUPPORT_OPENGL
	if( _source->data->size == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile shader with null source\n" );
		return nullptr;
	}

	sShader* shader = new sShader();

	GLenum type = GL_NONE;
	{
		switch( _type )
		{
		case wv::WV_SHADER_TYPE_FRAGMENT: type = GL_FRAGMENT_SHADER; break;
		case wv::WV_SHADER_TYPE_VERTEX:   type = GL_VERTEX_SHADER; break;
		}
	}

	WV_RETASSERT_GL( shader->handle, glCreateShader, type );

	std::string source = std::string( (char*)_source->data->data, _source->data->size ); // this assumes source is a string

	// GLSL specification (chapter 3.3) requires that #version be the first thing in a shader source
	// therefore '#if GL_ES' cannot be used in the shader itself
#ifdef EMSCRIPTEN
	source = "#version 300 es\n" + source;
#else
	source = "#version 460 core\n" + source;
#endif
	const char* sourcePtr = source.c_str();

	WV_ASSERT_GL( glShaderSource, shader->handle, 1, &sourcePtr, NULL );
	WV_ASSERT_ERR( "Failed to add shader source to program\n" );

	WV_ASSERT_GL( glCompileShader, shader->handle );

	int  success = 1;
	char infoLog[ 512 ];
	WV_ASSERT_GL( glGetShaderiv, shader->handle, GL_COMPILE_STATUS, &success );
	if( !success )
	{
		WV_ASSERT_GL( glGetShaderInfoLog, shader->handle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to compile shader\n %s \n", infoLog );
	}

	WV_ASSERT_ERR( "Failed to create shader\n" );

	return shader;
#else
	return nullptr;
#endif
}

void wv::cOpenGLGraphicsDevice::destroyShader( sShader* _shader )
{
#ifdef WV_SUPPORT_OPENGL
	WV_ASSERT_GL( glDeleteShader, _shader->handle );
	_shader->handle = 0;
#endif
}

wv::sShaderProgram* wv::cOpenGLGraphicsDevice::createProgram( sShaderProgramDesc* _desc )
{
	WV_RENDER_PRINT();

#ifdef WV_SUPPORT_OPENGL
	Debug::Print( "Creating Program '%s'\n", _desc->name.c_str() );

	sShaderProgram* program = new sShaderProgram();

	WV_RETASSERT_GL( program->handle, glCreateProgram );

	WV_ASSERT_ERR( "Failed to create program\n" );

	if( program->handle == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot link program with null handle\n" );
		return program;
	}

	if( _desc->pVertexShader )
	{
		WV_ASSERT_GL( glAttachShader, program->handle, _desc->pVertexShader->handle );
	}
	if( _desc->pFragmentShader )
	{
		WV_ASSERT_GL( glAttachShader, program->handle, _desc->pFragmentShader->handle );
	}
	
	WV_ASSERT_GL( glLinkProgram, program->handle );

	WV_ASSERT_ERR( "link\n" );

	int success = 0;
	char infoLog[ 512 ];
	WV_ASSERT_GL( glGetProgramiv, program->handle, GL_LINK_STATUS, &success );
	if( !success )
	{
		WV_ASSERT_GL( glGetProgramInfoLog, program->handle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to link program\n %s \n", infoLog );
	}

	GLint numActiveResources;
	glGetProgramInterfaceiv( program->handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numActiveResources );

	for( GLuint i = 0; i < numActiveResources; i++ )
	{
		GLenum props[ 1 ] = { GL_NAME_LENGTH };
		GLint res[ 1 ];
		glGetProgramResourceiv( program->handle, GL_UNIFORM_BLOCK, i, std::size( props ), props, std::size( res ), nullptr, res );

		std::string name( ( GLuint )res[ 0 ] - 1, '\0' );
		glGetProgramResourceName( program->handle, GL_UNIFORM_BLOCK, i, name.capacity() + 1, nullptr, name.data() );

		glUniformBlockBinding( program->handle, i, m_numTotalUniformBlocks );
		m_numTotalUniformBlocks++;

		sShaderBufferDesc desc;
		desc.name = name;
		

		cShaderBuffer* buf = createUniformBlock( program, &desc );
		program->shaderBuffers.push_back( buf );

		Debug::Print( Debug::WV_PRINT_DEBUG, "    - ShaderBuffer: %s\n", name.c_str() );
		// glShaderStorageBlockBinding(Handle, i, newBindingPoint);
	}

	/// TODO: move? I don't like having these vectors in linkProgram()
	//for( int i = 0; i < ( int )_uniformBlocks.size(); i++ )
	//{
	//	cShaderBuffer block = createUniformBlock( program, &_uniformBlocks[ i ] );
	//	_program->addUniformBlock( _uniformBlocks[ i ].name, block );
	//}

	WV_ASSERT_GL( glUseProgram, program->handle );

	// required for OpenGL < 4.2
	// could probably be skipped for OpenGL >= 4.2 but would require layout(binding=i) in the shader source
	//for( int i = 0; i < ( int )_textureUniforms.size(); i++ )
	//{
	//	unsigned int loc = glGetUniformLocation( programHandle, _textureUniforms[ i ].name.c_str() );
	//	WV_ASSERT_GL( glUniform1i, loc, _textureUniforms[ i ].index );
	//#ifdef WV_DEBUG
	//	assertGLError( "Failed to bind texture loc\n" );
	//#endif
	//}

	WV_ASSERT_GL( glUseProgram, 0 );

	return program;
#else
	return nullptr;
#endif
}

void wv::cOpenGLGraphicsDevice::destroyProgram( sShaderProgram* _program )
{
#ifdef WV_SUPPORT_OPENGL
	WV_ASSERT_GL( glDeleteProgram, _program->handle );
	_program->handle = 0;
#endif
}

void wv::cOpenGLGraphicsDevice::useProgram( sShaderProgram* _program )
{
#ifdef WV_SUPPORT_OPENGL
	if( m_activeProgram == _program )
		return;

	WV_ASSERT_GL( glUseProgram, _program ? _program->handle : 0 );
	m_activeProgram = _program;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::sGPUBuffer* wv::cOpenGLGraphicsDevice::createGPUBuffer( eGPUBufferType _type )
{
	WV_RENDER_PRINT();

#ifdef WV_SUPPORT_OPENGL
	sGPUBuffer* buffer = new sGPUBuffer();
	buffer->type = _type;

	glGenBuffers( 1, &buffer->handle );

	assertGLError( "Failed to create buffer\n" );

	return buffer;
#else 
	return nullptr;
#endif
}

void wv::cOpenGLGraphicsDevice::bufferData( sGPUBuffer* _buffer, void* _data, size_t _size )
{
#ifdef WV_SUPPORT_OPENGL
	/// TODO: more draw types
	//GLenum usage = GL_NONE;
	//switch ( _desc->type )
	//{
	//case wv::WV_PRIMITIVE_TYPE_STATIC: usage = GL_STATIC_DRAW; break;
	//}

	WV_ASSERT_GL( glBufferData, getGlBufferEnum( _buffer->type ), _size, _data, GL_STATIC_DRAW ) ;
	assertGLError( "Failed to buffer data\n" );
#endif
}

void wv::cOpenGLGraphicsDevice::destroyGPUBuffer( sGPUBuffer* _buffer )
{
#ifdef WV_SUPPORT_OPENGL
	WV_ASSERT_GL( glDeleteBuffers, 1, &_buffer->handle );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Mesh* wv::cOpenGLGraphicsDevice::createMesh( MeshDesc* _desc )
{
#ifdef WV_SUPPORT_OPENGL
	Mesh* mesh = new Mesh();
	/// TODO: remove?
	return mesh;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyMesh( Mesh** _mesh )
{
#ifdef WV_SUPPORT_OPENGL
	Debug::Print( Debug::WV_PRINT_DEBUG, "Destroyed mesh '%s'\n", (*_mesh)->name.c_str() );

	Mesh* mesh = *_mesh;
	for ( int i = 0; i < mesh->primitives.size(); i++ )
		destroyPrimitive( &mesh->primitives[ i ] );
	mesh->primitives.clear();
	mesh->triangles.clear();

	*_mesh = nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Primitive* wv::cOpenGLGraphicsDevice::createPrimitive( PrimitiveDesc* _desc, Mesh* _mesh )
{
#ifdef WV_SUPPORT_OPENGL
	Primitive* primitive = new Primitive();
	WV_ASSERT_GL( glGenVertexArrays, 1, &primitive->vaoHandle );
	WV_ASSERT_GL( glBindVertexArray, primitive->vaoHandle );

	primitive->vertexBuffer = createGPUBuffer( WV_BUFFER_TYPE_VERTEX );
	WV_ASSERT_GL( glBindBuffer, GL_ARRAY_BUFFER, primitive->vertexBuffer->handle );


	uint32_t count = _desc->sizeVertices / sizeof( Vertex );
	primitive->vertexBuffer->count = count;
	primitive->vertexBuffer->size  = _desc->sizeVertices;
	
	bufferData( primitive->vertexBuffer, _desc->vertices, primitive->vertexBuffer->size );

	if ( _desc->numIndices > 0 )
	{
		// create element buffer object
		primitive->indexBuffer = createGPUBuffer( WV_BUFFER_TYPE_INDEX );
		primitive->drawType = WV_PRIMITIVE_DRAW_TYPE_INDICES;
		
		primitive->indexBuffer->count = _desc->numIndices;
		WV_ASSERT_GL( glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, primitive->indexBuffer->handle );
		
		if( _desc->indices16 )
		{
			primitive->indexBuffer->size  = _desc->numIndices * sizeof( uint16_t );
			bufferData( primitive->indexBuffer, _desc->indices16, primitive->indexBuffer->size );
		}
		else if( _desc->indices32 )
		{
			primitive->indexBuffer->size  = _desc->numIndices * sizeof( uint32_t );
			bufferData( primitive->indexBuffer, _desc->indices32, primitive->indexBuffer->size );
		}
	}
	else
	{
		primitive->drawType = WV_PRIMITIVE_DRAW_TYPE_VERTICES;
	}

	int offset = 0;
	int stride = 0;
	for ( unsigned int i = 0; i < _desc->layout->numElements; i++ )
	{
		sVertexAttribute& element = _desc->layout->elements[ i ];
		stride += element.size;
	}

	for ( unsigned int i = 0; i < _desc->layout->numElements; i++ )
	{
		sVertexAttribute& element = _desc->layout->elements[ i ];

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

		WV_ASSERT_GL( glVertexAttribPointer, i, element.componentCount, type, element.normalized, stride, VPTRi32( offset ) );
		WV_ASSERT_GL( glEnableVertexAttribArray, i );

		offset += element.size;
	}

	WV_ASSERT_GL( glBindBuffer, GL_ARRAY_BUFFER, 0 );
	WV_ASSERT_GL( glBindVertexArray, 0 );

	primitive->vertexBuffer->stride = stride;
	
	_mesh->primitives.push_back( primitive );

	return primitive;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyPrimitive( Primitive** _primitive )
{
#ifdef WV_SUPPORT_OPENGL
	Primitive* pr = *_primitive;
	destroyGPUBuffer( pr->indexBuffer );
	destroyGPUBuffer( pr->vertexBuffer );

	WV_ASSERT_GL( glDeleteVertexArrays, 1, &pr->vaoHandle );
	delete pr;
	*_primitive = nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::createTexture( Texture* _pTexture, TextureDesc* _desc )
{
#ifdef WV_SUPPORT_OPENGL
	GLenum internalFormat = GL_R8;
	GLenum format = GL_RED;

	unsigned char* data = nullptr;
	if ( _pTexture->getData() )
	{
		data = _pTexture->getData();
		_desc->width = _pTexture->getWidth();
		_desc->height = _pTexture->getHeight();
		_desc->channels = static_cast<wv::TextureChannels>( _pTexture->getNumChannels() );
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
		WV_ASSERT_GL( glPixelStorei, GL_UNPACK_ALIGNMENT, 1 ); // 3 (channels) is not divisible by 4. Set pixel alignment to 1
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

	GLuint handle;
	WV_ASSERT_GL( glGenTextures, 1, &handle );
#ifdef WV_DEBUG
	assertGLError( "Failed to gen texture\n" );
#endif
	_pTexture->setHandle( handle );

	WV_ASSERT_GL( glBindTexture, GL_TEXTURE_2D, handle );
#ifdef WV_DEBUG
	assertGLError( "Failed to bind texture\n" );
#endif
	
	GLenum filter = GL_NEAREST;
	switch ( _desc->filtering )
	{
	case WV_TEXTURE_FILTER_NEAREST: filter = GL_NEAREST; break;
	case WV_TEXTURE_FILTER_LINEAR:  filter = GL_LINEAR; break;
	}
	
	WV_ASSERT_GL( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
	WV_ASSERT_GL( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
	WV_ASSERT_GL( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	WV_ASSERT_GL( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	GLenum type = GL_UNSIGNED_BYTE;
	switch ( _desc->format )
	{
	case wv::WV_TEXTURE_FORMAT_FLOAT: type = GL_FLOAT; break;
	case wv::WV_TEXTURE_FORMAT_INT:   type = GL_INT; break;
	}

	WV_ASSERT_GL( glTexImage2D, GL_TEXTURE_2D, 0, internalFormat, _desc->width, _desc->height, 0, format, type, data );
#ifdef WV_DEBUG
	assertGLError( "Failed to create Texture\n" );
#endif

	if ( _desc->generateMipMaps )
	{
		WV_ASSERT_GL( glGenerateMipmap, GL_TEXTURE_2D );
	}
	_pTexture->setWidth( _desc->width );
	_pTexture->setHeight( _desc->height );

	// Debug::Print( Debug::WV_PRINT_DEBUG, "Created texture %s\n", _pTexture->getName().c_str() );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyTexture( Texture** _texture )
{
#ifdef WV_SUPPORT_OPENGL
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Destroyed texture %s\n", (*_texture)->getName().c_str() );

	wv::Handle handle = ( *_texture )->getHandle();
	WV_ASSERT_GL( glDeleteTextures, 1, &handle );
	delete *_texture;
	*_texture = nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::bindTextureToSlot( Texture* _texture, unsigned int _slot )
{
#ifdef WV_SUPPORT_OPENGL
	/// TODO: some cleaner way of checking version/supported features
	if ( m_graphicsApiVersion.major == 4 && m_graphicsApiVersion.minor >= 5 ) // if OpenGL 4.5 or higher
	{
		WV_ASSERT_GL( glBindTextureUnit, _slot, _texture->getHandle() );
	}
	else 
	{
		WV_ASSERT_GL( glActiveTexture, GL_TEXTURE0 + _slot );
		WV_ASSERT_GL( glBindTexture, GL_TEXTURE_2D, _texture->getHandle() );
	}

	m_boundTextureSlots[ _slot ] = _texture->getHandle();
#endif
}
///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::drawPrimitive( Primitive* _primitive )
{
#ifdef WV_SUPPORT_OPENGL
	WV_ASSERT_GL( glBindVertexArray, _primitive->vaoHandle );
	
	std::vector<cShaderBuffer*>& shaderBuffers = m_activeProgram->shaderBuffers;
	for ( auto& buf : shaderBuffers )
	{
		WV_ASSERT_GL( glUniformBlockBinding, m_activeProgram->handle, buf->m_index, buf->m_bindingIndex);

		if( m_boundUniformBuffer != buf->m_bufferHandle )
		{
			WV_ASSERT_GL( glBindBuffer, GL_UNIFORM_BUFFER, buf->m_bufferHandle );
			m_boundUniformBuffer = buf->m_bufferHandle;
		}
		WV_ASSERT_GL( glBufferData, GL_UNIFORM_BUFFER, buf->m_bufferSize, buf->m_buffer, GL_DYNAMIC_DRAW );
	}
	
	/// TODO: change GL_TRIANGLES
	if ( _primitive->drawType == WV_PRIMITIVE_DRAW_TYPE_INDICES )
	{
		WV_ASSERT_GL( glDrawElements, GL_TRIANGLES, _primitive->indexBuffer->count, GL_UNSIGNED_INT, 0 );
	}
	else
	{ 
	#ifndef EMSCRIPTEN
		/// this does not work on WebGL
		wv::Handle vbo = _primitive->vertexBuffer->handle;
		size_t numVertices = _primitive->vertexBuffer->count;
		WV_ASSERT_GL( glBindVertexBuffer, 0, vbo, 0, _primitive->vertexBuffer->stride );
		WV_ASSERT_GL( glDrawArrays, GL_TRIANGLES, 0, numVertices );
	#else
		Debug::Print( Debug::WV_PRINT_FATAL, "glBindVertexBuffer is not supported on WebGL. Index Buffer is required\n" );
	#endif
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cOpenGLGraphicsDevice::getError( std::string* _out )
{
#ifdef WV_SUPPORT_OPENGL
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
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cShaderBuffer* wv::cOpenGLGraphicsDevice::createUniformBlock( sShaderProgram* _program, sShaderBufferDesc* _desc )
{
#ifdef WV_SUPPORT_OPENGL
	cShaderBuffer* sb = new cShaderBuffer();
	Handle programHandle = _program->handle;

	sb->m_bindingIndex = m_numTotalUniformBlocks;
	sb->name = _desc->name;

	WV_RETASSERT_GL( sb->m_index, glGetUniformBlockIndex, programHandle, _desc->name.c_str() );;
	WV_ASSERT_GL( glGetActiveUniformBlockiv, programHandle, sb->m_index, GL_UNIFORM_BLOCK_DATA_SIZE, &sb->m_bufferSize );

	sb->m_buffer = new char[ sb->m_bufferSize ];
	memset( sb->m_buffer, 0, sb->m_bufferSize );
	
	WV_ASSERT_GL( glGenBuffers, 1, &sb->m_bufferHandle );
	WV_ASSERT_GL( glBindBuffer, GL_UNIFORM_BUFFER, sb->m_bufferHandle );
	WV_ASSERT_GL( glBufferData, GL_UNIFORM_BUFFER, sb->m_bufferSize, 0, GL_DYNAMIC_DRAW );
	WV_ASSERT_GL( glBindBuffer, GL_UNIFORM_BUFFER, 0 );
	
	WV_ASSERT_GL( glBindBufferBase, GL_UNIFORM_BUFFER, sb->m_bindingIndex, sb->m_bufferHandle );

	m_numTotalUniformBlocks++;

	return sb;
#else
	return nullptr;
#endif
}


