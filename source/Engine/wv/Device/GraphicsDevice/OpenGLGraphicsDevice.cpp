#include "OpenGLGraphicsDevice.h"

#include <wv/Texture/Texture.h>
#include <wv/Material/Material.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Decl.h>

#include <wv/Memory/FileSystem.h>

#include <wv/Math/Transform.h>

#include <wv/Mesh/MeshResource.h>
#include <wv/Mesh/Mesh.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <wv/Device/DeviceContext.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif 

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

#define WV_HARD_ASSERT 0

#ifdef WV_DEBUG
	#define WV_VALIDATE_GL( _func ) if( _func == nullptr ) { Debug::Print( Debug::WV_PRINT_FATAL, "Missing function '%s'\n", #_func ); }

	#if WV_HARD_ASSERT
		#define WV_ASSERT_ERR( _msg ) if( !assertGLError( _msg ) ) throw std::runtime_error( _msg )
	#else
		#define WV_ASSERT_ERR( _msg ) assertGLError( _msg ) 
	#endif
#else
	#define WV_VALIDATE_GL( _func )
	#define WV_ASSERT_ERR( _msg ) 
#endif

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_OPENGL
static GLenum getGlBufferEnum( wv::eGPUBufferType _type )
{
	switch ( _type )
	{
	case wv::WV_BUFFER_TYPE_VERTEX:  return GL_ARRAY_BUFFER;         break;
	case wv::WV_BUFFER_TYPE_INDEX:   return GL_ELEMENT_ARRAY_BUFFER; break;
	case wv::WV_BUFFER_TYPE_UNIFORM: return GL_UNIFORM_BUFFER;       break;
	case wv::WV_BUFFER_TYPE_DYNAMIC_STORAGE: return GL_SHADER_STORAGE_BUFFER; break;
	}

	return GL_NONE;
}

static GLenum getGlBufferUsage( wv::eGPUBufferUsage _usage )
{
	switch( _usage )
	{
	case wv::WV_BUFFER_USAGE_STATIC_DRAW:  return GL_STATIC_DRAW; break;
	case wv::WV_BUFFER_USAGE_DYNAMIC_DRAW: return GL_DYNAMIC_DRAW; break;
	}

	return GL_NONE;
}
#endif
///////////////////////////////////////////////////////////////////////////////////////

wv::cOpenGLGraphicsDevice::cOpenGLGraphicsDevice()
{
	WV_TRACE();

}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cOpenGLGraphicsDevice::initialize( GraphicsDeviceDesc* _desc )
{
	WV_TRACE();

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
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );
	glEnable( GL_CULL_FACE );

	m_graphicsApiVersion.major = GLVersion.major;
	m_graphicsApiVersion.minor = GLVersion.minor;
	
	int numTextureUnits = 0;
	glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &numTextureUnits );

	glGenVertexArrays( 1, &m_vaoHandle);
	glBindVertexArray( m_vaoHandle );
	glBindVertexArray( 0 );

	return true;
#else
	return false;
#endif
}

void wv::cOpenGLGraphicsDevice::terminate()
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::onResize( int _width, int _height )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setViewport( int _width, int _height )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glViewport( 0, 0, _width, _height );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTarget* wv::cOpenGLGraphicsDevice::createRenderTarget( RenderTargetDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	RenderTargetDesc& desc = *_desc;
	RenderTarget* target = new RenderTarget();
	
	glGenFramebuffers( 1, &target->fbHandle );
	glBindFramebuffer( GL_FRAMEBUFFER, target->fbHandle );
	
	target->numTextures = desc.numTextures;
	GLenum* drawBuffers = new GLenum[ desc.numTextures ];
	target->pTextures = new sTexture[ desc.numTextures ];

	for ( int i = 0; i < desc.numTextures; i++ )
	{
		desc.pTextureDescs[ i ].width = desc.width;
		desc.pTextureDescs[ i ].height = desc.height;

		std::string texname = "buffer_tex" + std::to_string( i );

		target->pTextures[ i ] = createTexture( &desc.pTextureDescs[ i ] );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, target->pTextures[ i ].textureObjectHandle, 0 );

		drawBuffers[ i ] = GL_COLOR_ATTACHMENT0 + i;
	}

	glGenRenderbuffers( 1, &target->rbHandle );
	glBindRenderbuffer( GL_RENDERBUFFER, target->rbHandle );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, desc.width, desc.height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target->rbHandle );
	
	glDrawBuffers( desc.numTextures, drawBuffers );
	delete[] drawBuffers;
#ifdef WV_DEBUG
	int errcode = 0; 
	errcode = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	
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
	target->width  = desc.width;
	target->height = desc.height;

	glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return target;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyRenderTarget( RenderTarget** _renderTarget )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	RenderTarget* rt = *_renderTarget;

	glDeleteFramebuffers( 1, &rt->fbHandle );
	glDeleteRenderbuffers( 1, &rt->rbHandle );

	for ( int i = 0; i < rt->numTextures; i++ )
		destroyTexture( &rt->pTextures[ i ] );
	
	*_renderTarget = nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setRenderTarget( RenderTarget* _target )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if ( m_activeRenderTarget == _target )
		return;

	unsigned int handle = _target ? _target->fbHandle : 0;
	
	glBindFramebuffer( GL_FRAMEBUFFER, handle );
	if ( _target )
		glViewport( 0, 0, _target->width, _target->height );
	
	m_activeRenderTarget = _target;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setClearColor( const wv::cColor& _color )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glClearColor( _color.r, _color.g, _color.b, _color.a );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::clearRenderTarget( bool _color, bool _depth )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glClear( (GL_COLOR_BUFFER_BIT * _color) | (GL_DEPTH_BUFFER_BIT * _depth) );
#endif
}

wv::sShaderProgram* wv::cOpenGLGraphicsDevice::createProgram( sShaderProgramDesc* _desc )
{
	eShaderProgramType&   type   = _desc->type;
	sShaderProgramSource& source = _desc->source;

	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( source.data->size == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile shader with null source\n" );
		return nullptr;
	}

	sShaderProgram* program = new sShaderProgram();

	GLenum glType = GL_NONE;
	{
		switch( type )
		{
		case wv::WV_SHADER_TYPE_FRAGMENT: glType = GL_FRAGMENT_SHADER; break;
		case wv::WV_SHADER_TYPE_VERTEX:   glType = GL_VERTEX_SHADER;   break;
		}
	}

	std::string sourceStr = std::string( (char*)source.data->data, source.data->size ); // this assumes source is a string

	// GLSL specification (chapter 3.3) requires that #version be the first thing in a shader source
	// therefore '#if GL_ES' cannot be used in the shader itself
#ifdef EMSCRIPTEN
	sourceStr = "#version 300 es\n" + sourceStr;
#else
	sourceStr = "#version 460 core\n" + sourceStr;
#endif
	const char* sourcePtr = sourceStr.c_str();

	
	/// TODO: Use shader objects?

	program->handle = glCreateShaderProgramv( glType, 1, &sourcePtr );
	WV_ASSERT_ERR( "Failed create shader program\n" );

	/*
	int  success = 1;
	char infoLog[ 512 ];
	glGetShaderiv( program->handle, GL_COMPILE_STATUS, &success );
	if( !success )
	{
		glGetShaderInfoLog( program->handle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to compile shader\n %s \n", infoLog );
	}
	*/

	int success = 0;
	char infoLog[ 512 ];
	glGetProgramiv( program->handle, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( program->handle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to link program\n %s \n", infoLog );
	}

	WV_ASSERT_ERR( "Failed to create program\n" );

	GLint numActiveResources;
	glGetProgramInterfaceiv( program->handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numActiveResources );

	for ( GLuint i = 0; i < numActiveResources; i++ )
	{
		GLenum props[ 1 ] = { GL_NAME_LENGTH };
		GLint res[ 1 ];
		glGetProgramResourceiv( program->handle, GL_UNIFORM_BLOCK, i, std::size( props ), props, std::size( res ), nullptr, res );

		std::string name( (GLuint)res[ 0 ] - 1, '\0' );
		glGetProgramResourceName( program->handle, GL_UNIFORM_BLOCK, i, name.capacity() + 1, nullptr, name.data() );
		WV_ASSERT_ERR( "ERROR\n" );

		// create uniform buffer

		sGPUBufferDesc ubDesc;
		ubDesc.name  = name;
		ubDesc.type  = WV_BUFFER_TYPE_UNIFORM;
		ubDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		cGPUBuffer& buf = *createGPUBuffer( &ubDesc );

		sOpenGLBufferData* pUBData = new sOpenGLBufferData();
		buf.pPlatformData = pUBData;

		pUBData->bindingIndex = m_numTotalUniformBlocks;

		pUBData->blockIndex = glGetUniformBlockIndex( program->handle, name.c_str() );
		glGetActiveUniformBlockiv( program->handle, pUBData->blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &buf.size );
		
		WV_ASSERT_ERR( "ERROR\n" );

		glBindBuffer( GL_UNIFORM_BUFFER, buf.handle );
		allocateBuffer( &buf, buf.size );
		WV_ASSERT_ERR( "ERROR\n" );
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );
		
		glBindBufferBase( GL_UNIFORM_BUFFER, pUBData->bindingIndex, buf.handle );
		WV_ASSERT_ERR( "ERROR\n" );
		
		glUniformBlockBinding( program->handle, pUBData->blockIndex, pUBData->bindingIndex );
		WV_ASSERT_ERR( "ERROR\n" );
		
		m_numTotalUniformBlocks++;
		
		program->shaderBuffers.push_back( &buf );
	}

	
	GLint numActiveSSBOs;
	glGetProgramInterfaceiv( program->handle, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numActiveSSBOs );
	for ( GLuint i = 0; i < numActiveSSBOs; i++ )
	{
		GLenum props[ 1 ] = { GL_NAME_LENGTH };
		GLint res[ 1 ];
		glGetProgramResourceiv( program->handle, GL_SHADER_STORAGE_BLOCK, i, std::size( props ), props, std::size( res ), nullptr, res );

		std::string name( (GLuint)res[ 0 ] - 1, '\0' );
		glGetProgramResourceName( program->handle, GL_SHADER_STORAGE_BLOCK, i, name.capacity() + 1, nullptr, name.data() );
		WV_ASSERT_ERR( "ERROR\n" );

		// create ssbo

		sGPUBufferDesc ubDesc;
		ubDesc.name = name;
		ubDesc.type = WV_BUFFER_TYPE_DYNAMIC_STORAGE;
		ubDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		cGPUBuffer& buf = *createGPUBuffer( &ubDesc );

		sOpenGLBufferData* pUBData = new sOpenGLBufferData();
		buf.pPlatformData = pUBData;

		glBindBuffer( GL_SHADER_STORAGE_BUFFER, buf.handle );
		pUBData->bindingIndex = m_numTotalUniformBlocks;
		pUBData->blockIndex = glGetProgramResourceIndex( program->handle, GL_SHADER_STORAGE_BLOCK, name.data() );
		glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
		WV_ASSERT_ERR( "ERROR\n" );

		glBindBufferBase( GL_SHADER_STORAGE_BUFFER, pUBData->bindingIndex, buf.handle );
		WV_ASSERT_ERR( "ERROR\n" );
		
		glShaderStorageBlockBinding( program->handle, pUBData->blockIndex, pUBData->bindingIndex );
		WV_ASSERT_ERR( "ERROR\n" );

		m_numTotalUniformBlocks++;

		program->shaderBuffers.push_back( &buf );
	}


	return program;
#else
	return nullptr;
#endif
}

void wv::cOpenGLGraphicsDevice::destroyProgram( sShaderProgram* _pProgram )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glDeleteProgram( _pProgram->handle );

	for( auto& buffer : _pProgram->shaderBuffers )
		destroyGPUBuffer( buffer );
	
	WV_ASSERT_ERR( "ERROR\n" );
	_pProgram->handle = 0;
#endif
}

wv::sPipeline* wv::cOpenGLGraphicsDevice::createPipeline( sPipelineDesc* _desc )
{
	WV_TRACE();

	sPipelineDesc& desc = *_desc;
	
#ifdef WV_SUPPORT_OPENGL
	sPipeline& pipeline = *new sPipeline();
	
	glGenProgramPipelines( 1, &pipeline.handle );

	WV_ASSERT_ERR( "Failed to create pipeline\n" );
	if( pipeline.handle == 0 ) return &pipeline;
	
	if ( desc.pVertexProgram )
	{
		glUseProgramStages( pipeline.handle, GL_VERTEX_SHADER_BIT, (*desc.pVertexProgram)->handle );
		pipeline.pVertexProgram   = *desc.pVertexProgram;
	}
	if ( desc.pFragmentProgram )
	{
		glUseProgramStages( pipeline.handle, GL_FRAGMENT_SHADER_BIT, (*desc.pFragmentProgram)->handle );
		pipeline.pFragmentProgram = *desc.pFragmentProgram;
	}

	WV_ASSERT_ERR( "Failed to bind pipeline stages\n" );

	return &pipeline;
#else
	return nullptr;
#endif
}

void wv::cOpenGLGraphicsDevice::destroyPipeline( sPipeline* _pPipeline )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glDeleteProgramPipelines( 1, &_pPipeline->handle );
	WV_ASSERT_ERR( "ERROR\n" );

	_pPipeline->handle = 0;
#endif
}

void wv::cOpenGLGraphicsDevice::bindPipeline( sPipeline* _pPipeline )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( m_activePipeline == _pPipeline )
		return;

	glBindProgramPipeline( _pPipeline ? _pPipeline->handle : 0 );
	WV_ASSERT_ERR( "ERROR\n" );

	m_activePipeline = _pPipeline;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cGPUBuffer* wv::cOpenGLGraphicsDevice::createGPUBuffer( sGPUBufferDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	cGPUBuffer& buffer = *new cGPUBuffer();
	buffer.type  = _desc->type;
	buffer.usage = _desc->usage;
	buffer.name  = _desc->name;

	GLenum target = getGlBufferEnum( buffer.type );
	
	//glGenBuffers( 1, &buffer.handle );

	glCreateBuffers( 1, &buffer.handle );
	glBindBuffer( target, buffer.handle );

	assertGLError( "Failed to create buffer\n" );

	if ( _desc->size > 0 )
		allocateBuffer( &buffer, _desc->size );
	
	glBindBuffer( target, 0 );
	
	return &buffer;
#else 
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::bufferData( cGPUBuffer* _buffer )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	cGPUBuffer& buffer = *_buffer;

	if ( buffer.pData == nullptr || buffer.size == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot submit buffer with 0 data or size\n" );
		return;
	}

	if ( buffer.size > buffer.bufferedSize )
	{
		GLenum usage = getGlBufferUsage( buffer.usage );
		GLenum target = getGlBufferEnum( buffer.type );
		glBindBuffer( target, buffer.handle );
		// glNamedBufferData( buffer.handle, buffer.size, buffer.pData, usage );
		glBufferData( target, buffer.size, buffer.pData, usage );
		glBindBuffer( target, 0 );

		buffer.bufferedSize = buffer.size;
	}
	else
	{
		glNamedBufferSubData( buffer.handle, 0, buffer.size, buffer.pData );
	}

	
	WV_ASSERT_ERR( "Failed to buffer data\n" );
	
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::allocateBuffer( cGPUBuffer* _buffer, size_t _size )
{
	cGPUBuffer& buffer = *_buffer;

	if ( buffer.pData )
		delete[] buffer.pData;
	
	GLenum usage = getGlBufferUsage( buffer.usage );
	buffer.pData = new uint8_t[ _size ];
	buffer.size  = _size;
	buffer.bufferedSize = buffer.size;

	glNamedBufferData( buffer.handle, _size, 0, usage );

	WV_ASSERT_ERR( "Failed to allocate buffer\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyGPUBuffer( cGPUBuffer* _buffer )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glDeleteBuffers( 1, &_buffer->handle );

	if ( _buffer->pData )
	{
		delete[] _buffer->pData;
		_buffer->pData = nullptr;
	}

	if ( _buffer->pPlatformData )
	{
		delete _buffer->pPlatformData;
		_buffer->pPlatformData = nullptr;
	}

	delete _buffer;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::sMesh* wv::cOpenGLGraphicsDevice::createMesh( sMeshDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sMesh& mesh = *new sMesh();
	glGenVertexArrays( 1, &mesh.handle );
	glBindVertexArray( mesh.handle );

	WV_ASSERT_ERR( "ERROR\n" );

	sGPUBufferDesc vbDesc;
	vbDesc.name  = "vbo";
	vbDesc.type  = WV_BUFFER_TYPE_VERTEX;
	vbDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;
	vbDesc.size  = _desc->sizeVertices;
	mesh.pVertexBuffer = createGPUBuffer( &vbDesc );
	mesh.pMaterial = _desc->pMaterial;

	uint32_t count = _desc->sizeVertices / sizeof( Vertex );
	mesh.pVertexBuffer->count = count;
	
	glBindBuffer( GL_ARRAY_BUFFER, mesh.pVertexBuffer->handle );
	
	WV_ASSERT_ERR( "ERROR\n" );

	mesh.pVertexBuffer->buffer( (uint8_t*)_desc->vertices, _desc->sizeVertices );
	bufferData( mesh.pVertexBuffer );
	
	if ( _desc->numIndices > 0 )
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_INDICES;

		sGPUBufferDesc ibDesc;
		ibDesc.name  = "ebo";
		ibDesc.type  = WV_BUFFER_TYPE_INDEX;
		ibDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;

		mesh.pIndexBuffer = createGPUBuffer( &ibDesc );
		mesh.pIndexBuffer->count = _desc->numIndices;
		
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.pIndexBuffer->handle );
		
		WV_ASSERT_ERR( "ERROR\n" );

		if ( _desc->pIndices16 )
		{
			const size_t bufferSize = _desc->numIndices * sizeof( uint16_t );

			allocateBuffer( mesh.pIndexBuffer, bufferSize );
			mesh.pIndexBuffer->buffer( _desc->pIndices16, bufferSize );
		}
		else if ( _desc->pIndices32 )
		{
			const size_t bufferSize = _desc->numIndices * sizeof( uint32_t );

			allocateBuffer( mesh.pIndexBuffer, bufferSize );
			mesh.pIndexBuffer->buffer( _desc->pIndices32, bufferSize );
		}

		bufferData( mesh.pIndexBuffer );
	}
	else
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_VERTICES;
	}
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
	
	WV_ASSERT_ERR( "ERROR\n" );

	if( _desc->pParentTransform != nullptr )
		_desc->pParentTransform->addChild( &mesh.transform );
	
	if( _desc->deleteData )
	{
		if( _desc->vertices )   { delete[] _desc->vertices; }
		if( _desc->pIndices16 ) { delete[] _desc->pIndices16; }
		if( _desc->pIndices32 ) { delete[] _desc->pIndices32; }
	}
	/*
	*/

	return &mesh;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyMesh( sMesh* _pMesh )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sMesh& pr = *_pMesh;
	destroyGPUBuffer( pr.pIndexBuffer );
	destroyGPUBuffer( pr.pVertexBuffer );

	glDeleteVertexArrays( 1, &pr.handle );
	WV_ASSERT_ERR( "ERROR\n" );
	delete _pMesh;
#endif
}

wv::sTexture wv::cOpenGLGraphicsDevice::createTexture( sTextureDesc* _pDesc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	GLenum internalFormat = GL_R8;
	GLenum format = GL_RED;

	sTextureDesc& desc = *_pDesc;
	sTexture texture;
	
	switch ( desc.channels )
	{
	case wv::WV_TEXTURE_CHANNELS_R:
		format = GL_RED;
		switch ( desc.format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internalFormat = GL_R8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_R32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internalFormat = GL_R32I;
			format = GL_RED_INTEGER;
			break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RG:
		format = GL_RG;
		switch ( desc.format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internalFormat = GL_RG8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_RG32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internalFormat = GL_RG32I;
			format = GL_RG_INTEGER;
			break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RGB:
		format = GL_RGB;
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // 3 (channels) is not divisible by 4. Set pixel alignment to 1
		switch ( desc.format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internalFormat = GL_RGB8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_RGB32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internalFormat = GL_RGB32I;
			format = GL_RGB_INTEGER;
			break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RGBA:
		format = GL_RGBA;
		switch ( desc.format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internalFormat = GL_RGBA8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internalFormat = GL_RGBA32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internalFormat = GL_RGBA32I;
			format = GL_RGBA_INTEGER;
			break;
		}
		break;
	}

	glGenTextures( 1, &texture.textureObjectHandle );
	WV_ASSERT_ERR( "Failed to gen texture\n" );

	glBindTexture( GL_TEXTURE_2D, texture.textureObjectHandle );

	WV_ASSERT_ERR( "Failed to bind texture\n" );

	GLenum minFilter = GL_NEAREST;
	GLenum magFilter = GL_NEAREST;

	if ( desc.generateMipMaps )
	{
		switch ( desc.filtering )
		{
		case WV_TEXTURE_FILTER_NEAREST: magFilter = GL_NEAREST; minFilter = GL_NEAREST_MIPMAP_NEAREST; break;
		case WV_TEXTURE_FILTER_LINEAR:  magFilter = GL_LINEAR;  minFilter = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else
	{
		switch ( desc.filtering )
		{
		case WV_TEXTURE_FILTER_NEAREST: magFilter = GL_NEAREST; minFilter = GL_NEAREST; break;
		case WV_TEXTURE_FILTER_LINEAR:  magFilter = GL_LINEAR;  minFilter = GL_LINEAR; break;
		}
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	GLenum type = GL_UNSIGNED_BYTE;
	switch ( desc.format )
	{
	case wv::WV_TEXTURE_FORMAT_FLOAT: type = GL_FLOAT; break;
	case wv::WV_TEXTURE_FORMAT_INT: type = GL_INT; break;
	}

	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, desc.width, desc.height, 0, format, type, nullptr );
	WV_ASSERT_ERR( "Failed to create texture\n" );

	sOpenGLTextureData* pPData = new sOpenGLTextureData();
	pPData->filter = minFilter;
	pPData->format = format;
	pPData->internalFormat = internalFormat;
	pPData->type = type;

	texture.pPlatformData = pPData;
	texture.width  = desc.width;
	texture.height = desc.height;
	
	return texture;
	// Debug::Print( Debug::WV_PRINT_DEBUG, "Created texture %s\n", _pTexture->getName().c_str() );
#endif
}

void wv::cOpenGLGraphicsDevice::bufferTextureData( sTexture* _pTexture, void* _pData, bool _generateMipMaps )
{
	sOpenGLTextureData* pPData = (sOpenGLTextureData*)_pTexture->pPlatformData;

	glBindTexture( GL_TEXTURE_2D, _pTexture->textureObjectHandle );
	
	glTexImage2D( GL_TEXTURE_2D, 0, pPData->internalFormat, _pTexture->width, _pTexture->height, 0, pPData->format, pPData->type, _pData );
	if ( _generateMipMaps )
	{
		glGenerateMipmap( GL_TEXTURE_2D );

		WV_ASSERT_ERR( "Failed to generate MipMaps\n" );
	}
	
	glBindTexture( GL_TEXTURE_2D, 0 );

	_pTexture->pData = (uint8_t*)_pData;

#ifdef WV_DEBUG
	assertGLError( "Failed to buffer Texture data\n" );
#endif
}

void wv::cOpenGLGraphicsDevice::destroyTexture( sTexture* _pTexture )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glDeleteTextures( 1, &_pTexture->textureObjectHandle );
	if( _pTexture->pData )
	{
		delete[] _pTexture->pData;
		_pTexture->pData = nullptr;
	}

	if( _pTexture->pPlatformData )
	{
		delete _pTexture->pPlatformData;
		_pTexture->pPlatformData = nullptr;
	}
#endif
}

void wv::cOpenGLGraphicsDevice::bindTextureToSlot( sTexture* _pTexture, unsigned int _slot )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	/// TODO: some cleaner way of checking version/supported features
	if ( m_graphicsApiVersion.major == 4 && m_graphicsApiVersion.minor >= 5 ) // if OpenGL 4.5 or higher
	{
		glBindTextureUnit( _slot, _pTexture->textureObjectHandle );

		WV_ASSERT_ERR( "ERROR\n" );
	}
	else
	{
		glActiveTexture( GL_TEXTURE0 + _slot );
		glBindTexture( GL_TEXTURE_2D, _pTexture->textureObjectHandle );

		WV_ASSERT_ERR( "ERROR\n" );
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::bindVertexArray( sMesh* _pMesh )
{
	if ( _pMesh->pMaterial && _pMesh->pMaterial->isComplete() )
	{
		wv::cGPUBuffer* SbVertices = _pMesh->pMaterial->getPipeline()->getShaderBuffer( "SbVertices" );
		if ( SbVertices && _pMesh->pVertexBuffer->pData )
			SbVertices->buffer( _pMesh->pVertexBuffer->pData, _pMesh->pVertexBuffer->size );
	}

	glBindVertexArray( _pMesh->handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setFillMode( eFillMode _mode )
{
	switch ( _mode )
	{
	case WV_FILL_MODE_SOLID:     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  break;
	case WV_FILL_MODE_WIREFRAME: glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  break;
	case WV_FILL_MODE_POINTS:    glPolygonMode( GL_FRONT_AND_BACK, GL_POINT ); break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::draw( sMesh* _pMesh )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sMesh& rMesh = *_pMesh;

	bindVertexArray( _pMesh );
	
	WV_ASSERT_ERR( "ERROR\n" );

	std::vector<cGPUBuffer*>& shaderBuffers = m_activePipeline->pVertexProgram->shaderBuffers;
	for ( auto& buf : shaderBuffers )
		bufferData( buf );
	
	/// TODO: change GL_TRIANGLES
	if ( rMesh.drawType == WV_MESH_DRAW_TYPE_INDICES )
	{
		drawIndexed( rMesh.pIndexBuffer->count );
	}
	else
	{ 
	#ifndef EMSCRIPTEN
		/// this does not work on WebGL
		wv::Handle vbo = rMesh.pVertexBuffer->handle;
		size_t numVertices = rMesh.pVertexBuffer->count;
		glBindVertexBuffer( 0, vbo, 0, rMesh.pVertexBuffer->stride );
		glDrawArrays( GL_TRIANGLES, 0, numVertices );
	#else
		Debug::Print( Debug::WV_PRINT_FATAL, "glBindVertexBuffer is not supported on WebGL. Index Buffer is required\n" );
	#endif
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::drawIndexed( uint32_t _numIndices )
{
	WV_TRACE();

	/// TODO: allow for other draw modes
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml#:~:text=Parameters-,mode,-Specifies%20what%20kind

	glDrawElements( GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::drawIndexedInstances( uint32_t _numIndices, uint32_t _numInstances )
{
	WV_TRACE();

	glDrawElementsInstanced( GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0, _numInstances );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cOpenGLGraphicsDevice::getError( std::string* _out )
{
	WV_TRACE();

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
		
		case GL_STACK_OVERFLOW:  *_out = "GL_STACK_OVERFLOW";  break;
		case GL_STACK_UNDERFLOW: *_out = "GL_STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:   *_out = "GL_OUT_OF_MEMORY";   break;
		case GL_CONTEXT_LOST:    *_out = "GL_OUT_OF_MEMORY";   break;

		case GL_INVALID_FRAMEBUFFER_OPERATION: *_out = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		}
	}

	return hasError;
#else
	return false;
#endif
}
