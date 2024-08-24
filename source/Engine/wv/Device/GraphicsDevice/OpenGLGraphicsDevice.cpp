#include "OpenGLGraphicsDevice.h"

#include <wv/Assets/Texture.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Debug/Print.h>
#include <wv/Decl.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Primitive/Primitive.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <wv/Device/DeviceContext.h>

#include <glad/glad.h>
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
#else
#define WV_ASSERT_GL( _func, ... ) _func( __VA_ARGS__ )
#define WV_RETASSERT_GL( _ret, _func, ... ) _ret = _func( __VA_ARGS__ )
#endif

///////////////////////////////////////////////////////////////////////////////////////

static inline GLenum getGlBufferEnum( wv::eGPUBufferType _type )
{
	switch ( _type )
	{
	case wv::WV_BUFFER_TYPE_VERTEX: return GL_ARRAY_BUFFER;         break;
	case wv::WV_BUFFER_TYPE_INDEX:  return GL_ELEMENT_ARRAY_BUFFER; break;
	}

	return GL_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cOpenGLGraphicsDevice::cOpenGLGraphicsDevice()
{
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cOpenGLGraphicsDevice::initialize( GraphicsDeviceDesc* _desc )
{
	/// TODO: make configurable

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
	
	m_boundTextureSlots.assign( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0 );

	return true;
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
	WV_ASSERT_GL( glViewport, 0, 0, _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTarget* wv::cOpenGLGraphicsDevice::createRenderTarget( RenderTargetDesc* _desc )
{
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
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyRenderTarget( RenderTarget** _renderTarget )
{
	RenderTarget* rt = *_renderTarget;

	WV_ASSERT_GL( glDeleteFramebuffers, 1, &rt->fbHandle );
	WV_ASSERT_GL( glDeleteRenderbuffers, 1, &rt->rbHandle );

	for ( int i = 0; i < rt->numTextures; i++ )
	{
		wv::Handle handle = rt->textures[ i ]->getHandle();
		WV_ASSERT_GL( glDeleteTextures, 1, &handle );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setRenderTarget( RenderTarget* _target )
{
	if ( m_activeRenderTarget == _target )
		return;

	unsigned int handle = _target ? _target->fbHandle : 0;
	
	WV_ASSERT_GL( glBindFramebuffer, GL_FRAMEBUFFER, handle );
	if ( _target )
	{
		WV_ASSERT_GL( glViewport, 0, 0, _target->width, _target->height );
	}
	
	m_activeRenderTarget = _target;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setClearColor( const wv::cColor& _color )
{
	WV_ASSERT_GL( glClearColor, _color.r, _color.g, _color.b, _color.a );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::clearRenderTarget( bool _color, bool _depth )
{
	WV_ASSERT_GL( glClear, (GL_COLOR_BUFFER_BIT * _color) | (GL_DEPTH_BUFFER_BIT * _depth) );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cOpenGLGraphicsDevice::createShader( cShader* _shader, eShaderType _type )
{
	GLenum type = GL_NONE;
	{
		switch ( _type )
		{
		case wv::WV_SHADER_TYPE_FRAGMENT: type = GL_FRAGMENT_SHADER; break;
		case wv::WV_SHADER_TYPE_VERTEX:   type = GL_VERTEX_SHADER; break;
		}
	}

	wv::Handle handle{};
	WV_RETASSERT_GL( handle, glCreateShader, type );

#ifdef WV_DEBUG
	assertGLError( "Failed to create shader\n" );
#endif
	
	_shader->setHandle( handle );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyShader( cShader* _shader )
{
	WV_ASSERT_GL( glDeleteShader, _shader->getHandle() );
	_shader->setHandle( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::compileShader( cShader* _shader )
{
	if ( !_shader )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile null shader\n" );
		return;
	}
	
	std::string source = _shader->getSource();
	if ( source == "" )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile shader '%s' with null source\n", _shader->getName().c_str() );
		return;
	}

	// GLSL specification (chapter 3.3) requires that #version be the first thing in a shader source
	// therefore '#if GL_ES' cannot be used in the shader itself
	/// TODO: move /// This will keep adding if compileShader is called more than once on the same shader
#ifdef EMSCRIPTEN
	source = "#version 300 es\n" + source;
#else
	source = "#version 460 core\n" + source;
#endif
	const char* sourcePtr = source.c_str();
	_shader->setSource( source );

	wv::Handle shaderHandle = _shader->getHandle();
	WV_ASSERT_GL( glShaderSource, shaderHandle, 1, &sourcePtr, NULL);
#ifdef WV_DEBUG
	assertGLError( "Failed to add shader source to program\n" );
#endif

	WV_ASSERT_GL( glCompileShader, shaderHandle );
	
	int  success = 1;
	char infoLog[ 512 ];
	WV_ASSERT_GL( glGetShaderiv, shaderHandle, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		WV_ASSERT_GL( glGetShaderInfoLog, shaderHandle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to compile shader '%s'\n %s \n", _shader->getName().c_str(), infoLog);
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::createProgram( wv::cShaderProgram* _program, const std::string& _name )
{
	wv::Handle handle{};
	WV_RETASSERT_GL( handle, glCreateProgram );
#ifdef WV_DEBUG
	assertGLError( "Failed to create program\n" );
#endif
	_program->setHandle( handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyProgram( cShaderProgram* _program )
{
	WV_ASSERT_GL( glDeleteProgram, _program->getHandle() );
	_program->setHandle( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::linkProgram( cShaderProgram* _program, std::vector<UniformBlockDesc> _uniformBlocks, std::vector<Uniform> _textureUniforms )
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
		WV_ASSERT_GL( glAttachShader, programHandle, shaders[ i ]->getHandle() );
	#ifdef WV_DEBUG
		assertGLError( "attach\n" );
	#endif
	}
	WV_ASSERT_GL( glLinkProgram, programHandle );
#ifdef WV_DEBUG
	assertGLError( "link\n" );
#endif

	int success = 0;
	char infoLog[ 512 ];
	WV_ASSERT_GL( glGetProgramiv, programHandle, GL_LINK_STATUS, &success );
	if ( !success )
	{
		WV_ASSERT_GL( glGetProgramInfoLog, programHandle, 512, NULL, infoLog );
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

	WV_ASSERT_GL( glUseProgram, programHandle );

	// required for OpenGL < 4.2
	// could probably be skipped for OpenGL >= 4.2 but would require layout(binding=i) in the shader source
	for ( int i = 0; i < (int)_textureUniforms.size(); i++ )
	{
		unsigned int loc = glGetUniformLocation( programHandle, _textureUniforms[ i ].name.c_str() );
		WV_ASSERT_GL( glUniform1i, loc, _textureUniforms[ i ].index );
	#ifdef WV_DEBUG
		assertGLError( "Failed to bind texture loc\n" );
	#endif
	}

	WV_ASSERT_GL( glUseProgram, 0 );

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::useProgram( cShaderProgram* _program )
{
	if ( m_activeProgram == _program )
		return;

	WV_ASSERT_GL( glUseProgram, _program ? _program->getHandle() : 0 );
	m_activeProgram = _program;
}

wv::sGPUBuffer* wv::cOpenGLGraphicsDevice::createGPUBuffer( eGPUBufferType _type )
{
	sGPUBuffer* buffer = new sGPUBuffer();
	buffer->type = _type;

	glGenBuffers( 1, &buffer->handle );

	assertGLError( "Failed to create buffer\n" );

	return buffer;
}

void wv::cOpenGLGraphicsDevice::bufferData( sGPUBuffer* _buffer, void* _data, size_t _size )
{
	/// TODO: more draw types
	//GLenum usage = GL_NONE;
	//switch ( _desc->type )
	//{
	//case wv::WV_PRIMITIVE_TYPE_STATIC: usage = GL_STATIC_DRAW; break;
	//}

	WV_ASSERT_GL( glBufferData, getGlBufferEnum( _buffer->type ), _size, _data, GL_STATIC_DRAW ) ;
	assertGLError( "Failed to buffer data\n" );
}

void wv::cOpenGLGraphicsDevice::destroyGPUBuffer( sGPUBuffer* _buffer )
{
	WV_ASSERT_GL( glDeleteBuffers, 1, &_buffer->handle );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Mesh* wv::cOpenGLGraphicsDevice::createMesh( MeshDesc* _desc )
{
	Mesh* mesh = new Mesh();
	/// TODO: remove?
	return mesh;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyMesh( Mesh** _mesh )
{
	Debug::Print( Debug::WV_PRINT_DEBUG, "Destroyed mesh '%s'\n", (*_mesh)->name.c_str() );

	Mesh* mesh = *_mesh;
	for ( int i = 0; i < mesh->primitives.size(); i++ )
		destroyPrimitive( &mesh->primitives[ i ] );
	mesh->primitives.clear();
	mesh->triangles.clear();

	*_mesh = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Primitive* wv::cOpenGLGraphicsDevice::createPrimitive( PrimitiveDesc* _desc, Mesh* _mesh )
{
	Primitive* primitive = new Primitive();
	WV_ASSERT_GL( glGenVertexArrays, 1, &primitive->vaoHandle );
	WV_ASSERT_GL( glBindVertexArray, primitive->vaoHandle );

	primitive->vertexBuffer = createGPUBuffer( WV_BUFFER_TYPE_VERTEX );
	WV_ASSERT_GL( glBindBuffer, GL_ARRAY_BUFFER, primitive->vertexBuffer->handle );

	primitive->vertexBuffer->count = _desc->vertices.size();
	primitive->vertexBuffer->size  = _desc->vertices.size() * sizeof( Vertex );

	bufferData( primitive->vertexBuffer, _desc->vertices.data(), primitive->vertexBuffer->size );

	if ( _desc->indices.size() > 0 )
	{
		// create element buffer object
		primitive->indexBuffer = createGPUBuffer( WV_BUFFER_TYPE_INDEX );
		primitive->drawType = WV_PRIMITIVE_DRAW_TYPE_INDICES;
		
		primitive->indexBuffer->count = _desc->indices.size();
		primitive->indexBuffer->size  = _desc->indices.size() * sizeof( uint32_t );

		WV_ASSERT_GL( glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, primitive->indexBuffer->handle );
		bufferData( primitive->indexBuffer, _desc->indices.data(), primitive->indexBuffer->size );

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

		WV_ASSERT_GL( glVertexAttribPointer, i, element.num, type, element.normalized, stride, VPTRi32( offset ) );
		WV_ASSERT_GL( glEnableVertexAttribArray, i );

		offset += element.size;
	}

	WV_ASSERT_GL( glBindBuffer, GL_ARRAY_BUFFER, 0 );
	WV_ASSERT_GL( glBindVertexArray, 0 );

	primitive->vertexBuffer->stride = stride;
	
	_mesh->primitives.push_back( primitive );

	return primitive;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyPrimitive( Primitive** _primitive )
{
	Primitive* pr = *_primitive;
	destroyGPUBuffer( pr->indexBuffer );
	destroyGPUBuffer( pr->vertexBuffer );

	WV_ASSERT_GL( glDeleteVertexArrays, 1, &pr->vaoHandle );
	delete pr;
	*_primitive = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::createTexture( Texture* _pTexture, TextureDesc* _desc )
{
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
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyTexture( Texture** _texture )
{
	Debug::Print( Debug::WV_PRINT_DEBUG, "Destroyed texture\n" );

	wv::Handle handle = ( *_texture )->getHandle();
	WV_ASSERT_GL( glDeleteTextures, 1, &handle );
	delete *_texture;
	*_texture = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::bindTextureToSlot( Texture* _texture, unsigned int _slot )
{
	if ( m_boundTextureSlots[ _slot ] == _texture->getHandle() )
		return;

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
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::draw( Mesh* _mesh )
{
	if ( !_mesh )
		return;

	for ( size_t i = 0; i < _mesh->primitives.size(); i++ )
	{
		cMaterial* mat = _mesh->primitives[ i ]->material;
		if( mat )
		{
			if( !mat->isLoaded() || !mat->isCreated() )
			{
				continue;
			}

			mat->setAsActive( this );
			mat->setInstanceUniforms( _mesh );
			drawPrimitive( _mesh->primitives[ i ] );
		}
		else
		{
			drawPrimitive( _mesh->primitives[ i ] );
		}
		


	}

}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cOpenGLGraphicsDevice::getError( std::string* _out )
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

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::drawPrimitive( Primitive* _primitive )
{
	WV_ASSERT_GL( glBindVertexArray, _primitive->vaoHandle );
	
	UniformBlockMap* uniformBlocks = m_activeProgram->getUniformBlockMap();
	for ( auto& block : *uniformBlocks )
	{
		WV_ASSERT_GL( glUniformBlockBinding, m_activeProgram->getHandle(), block.second.m_index, block.second.m_bindingIndex);

		if( m_boundUniformBuffer != block.second.m_bufferHandle )
		{
			WV_ASSERT_GL( glBindBuffer, GL_UNIFORM_BUFFER, block.second.m_bufferHandle );
			m_boundUniformBuffer = block.second.m_bufferHandle;
		}
		WV_ASSERT_GL( glBufferData, GL_UNIFORM_BUFFER, block.second.m_bufferSize, block.second.m_buffer, GL_DYNAMIC_DRAW );
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
}

///////////////////////////////////////////////////////////////////////////////////////

wv::UniformBlock wv::cOpenGLGraphicsDevice::createUniformBlock( cShaderProgram* _program, UniformBlockDesc* _desc )
{
	UniformBlock block;
	Handle programHandle = _program->getHandle();

	block.m_bindingIndex = m_numTotalUniformBlocks;
	
	block.m_index = 0;
	WV_RETASSERT_GL( block.m_index, glGetUniformBlockIndex, programHandle, _desc->name.c_str() );;
	WV_ASSERT_GL( glGetActiveUniformBlockiv, programHandle, block.m_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block.m_bufferSize );

	block.m_buffer = new char[ block.m_bufferSize ];
	memset( block.m_buffer, 0, block.m_bufferSize );
	
	WV_ASSERT_GL( glGenBuffers, 1, &block.m_bufferHandle );
	WV_ASSERT_GL( glBindBuffer, GL_UNIFORM_BUFFER, block.m_bufferHandle );
	WV_ASSERT_GL( glBufferData, GL_UNIFORM_BUFFER, block.m_bufferSize, 0, GL_DYNAMIC_DRAW );
	WV_ASSERT_GL( glBindBuffer, GL_UNIFORM_BUFFER, 0 );
	
	WV_ASSERT_GL( glBindBufferBase, GL_UNIFORM_BUFFER, block.m_bindingIndex, block.m_bufferHandle );

	std::vector<unsigned int> indices( _desc->uniforms.size() );
	std::vector<int> offsets( _desc->uniforms.size() );
	
	std::vector<const char*> uniformNames;
	for ( size_t i = 0; i < _desc->uniforms.size(); i++ )
		uniformNames.push_back( _desc->uniforms[ i ].name.c_str() ); // lifetime issues?
	
	WV_ASSERT_GL( glGetUniformIndices, programHandle, (GLsizei)_desc->uniforms.size(), uniformNames.data(), indices.data() );
	WV_ASSERT_GL( glGetActiveUniformsiv, programHandle, (GLsizei)_desc->uniforms.size(), indices.data(), GL_UNIFORM_OFFSET, offsets.data());

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
