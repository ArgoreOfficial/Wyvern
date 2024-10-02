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
		#define WV_ASSERT_GL( _func ) _func; if( !assertGLError( #_func ) ) throw std::runtime_error( #_func )
	#else
		#define WV_ASSERT_GL( _func ) _func; assertGLError( #_func )
	#endif
#else
	#define WV_VALIDATE_GL( _func )
	#define WV_ASSERT_GL( _func ) _func
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
	//case WV_GRAPHICS_API_OPENGL_ES1: initRes = gladLoadGLES1Loader( _desc->loadProc ); break;
	//case WV_GRAPHICS_API_OPENGL_ES2: initRes = gladLoadGLES2Loader( _desc->loadProc ); break;
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

	glCreateVertexArrays( 1, &m_vaoHandle );

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

void wv::cOpenGLGraphicsDevice::beginRender()
{
	iGraphicsDevice::beginRender();

	glBindVertexArray( m_vaoHandle );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::cOpenGLGraphicsDevice::createRenderTarget( sRenderTargetDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sRenderTargetDesc& desc = *_desc;
	RenderTargetID id = m_renderTargets.allocate();
	sRenderTarget& target = m_renderTargets.get( id );
	
	glCreateFramebuffers( 1, &target.fbHandle );
	
	target.numTextures = desc.numTextures;
	GLenum* drawBuffers = new GLenum[ desc.numTextures ];
	target.pTextures = new sTexture[ desc.numTextures ];

	for ( int i = 0; i < desc.numTextures; i++ )
	{
		desc.pTextureDescs[ i ].width = desc.width;
		desc.pTextureDescs[ i ].height = desc.height;

		std::string texname = "buffer_tex" + std::to_string( i );

		target.pTextures[ i ] = createTexture( &desc.pTextureDescs[ i ] );

		glNamedFramebufferTexture( target.fbHandle, GL_COLOR_ATTACHMENT0 + i, target.pTextures[ i ].textureObjectHandle, 0 );
		drawBuffers[ i ] = GL_COLOR_ATTACHMENT0 + i;
	}

	glCreateRenderbuffers( 1, &target.rbHandle );

	glNamedRenderbufferStorage( target.rbHandle, GL_DEPTH_COMPONENT24, desc.width, desc.height );
	glNamedFramebufferRenderbuffer( target.fbHandle, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target.rbHandle );

	glNamedFramebufferDrawBuffers( target.fbHandle, desc.numTextures, drawBuffers );

	delete[] drawBuffers;
#ifdef WV_DEBUG
	int errcode = 0;
	errcode = glCheckNamedFramebufferStatus( target.fbHandle, GL_FRAMEBUFFER );
	
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

		destroyRenderTarget( id );
		
		return RenderTargetID{ 0 };
	}
#endif
	target.width  = desc.width;
	target.height = desc.height;

	return id;
#else
	return RenderTargetID{ 0 };
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sRenderTarget& rt = m_renderTargets.get( _renderTargetID );

	glDeleteFramebuffers( 1, &rt.fbHandle );
	glDeleteRenderbuffers( 1, &rt.rbHandle );

	for ( int i = 0; i < rt.numTextures; i++ )
		destroyTexture( &rt.pTextures[ i ] );

	m_renderTargets.deallocate( _renderTargetID );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if ( m_activeRenderTarget == _renderTargetID )
		return;

	sRenderTarget& rt = m_renderTargets.get( _renderTargetID );
	unsigned int handle = rt.fbHandle;

	glBindFramebuffer( GL_FRAMEBUFFER, handle );
	glViewport( 0, 0, rt.width, rt.height );
	
	m_activeRenderTarget = _renderTargetID;
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

wv::ShaderProgramID wv::cOpenGLGraphicsDevice::createProgram( sShaderProgramDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	ShaderProgramID id = m_shaderPrograms.allocate();

	eShaderProgramType&   type   = _desc->type;
	sShaderProgramSource& source = _desc->source;

	if( source.data->size == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile shader with null source\n" );
		return ShaderProgramID{ 0 };
	}

	// sShaderProgram* program = new sShaderProgram();
	sShaderProgram& program = m_shaderPrograms.get( id );
	program.type = type;
	program.source = source;

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

	program.handle = WV_ASSERT_GL( glCreateShaderProgramv( glType, 1, &sourcePtr ) );
	
	int success = 0;
	char infoLog[ 512 ];
	glGetProgramiv( program.handle, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( program.handle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to link program\n %s \n", infoLog );
	}

	GLint numActiveResources;
	glGetProgramInterfaceiv( program.handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numActiveResources );

	for ( GLuint i = 0; i < numActiveResources; i++ )
	{
		GLenum props[ 1 ] = { GL_NAME_LENGTH };
		GLint res[ 1 ];
		glGetProgramResourceiv( program.handle, GL_UNIFORM_BLOCK, i, std::size( props ), props, std::size( res ), nullptr, res );

		std::string name( (GLuint)res[ 0 ] - 1, '\0' );
		WV_ASSERT_GL( glGetProgramResourceName( program.handle, GL_UNIFORM_BLOCK, i, name.capacity() + 1, nullptr, name.data() ) );

		// create uniform buffer

		sGPUBufferDesc ubDesc;
		ubDesc.name  = name;
		ubDesc.type  = WV_BUFFER_TYPE_UNIFORM;
		ubDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		cGPUBuffer& buf = *createGPUBuffer( &ubDesc );

		sOpenGLBufferData* pUBData = new sOpenGLBufferData();
		buf.pPlatformData = pUBData;

		pUBData->bindingIndex = getBufferBindingIndex();

		pUBData->blockIndex = glGetUniformBlockIndex( program.handle, name.c_str() );
		WV_ASSERT_GL( glGetActiveUniformBlockiv( program.handle, pUBData->blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &buf.size ) );
		
		allocateBuffer( &buf, buf.size );
		
		WV_ASSERT_GL( glBindBufferBase( GL_UNIFORM_BUFFER, pUBData->bindingIndex, buf.handle ) );
		
		WV_ASSERT_GL( glUniformBlockBinding( program.handle, pUBData->blockIndex, pUBData->bindingIndex ) );
		
		program.shaderBuffers.push_back( &buf );
	}

	
	GLint numActiveSSBOs;
	glGetProgramInterfaceiv( program.handle, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numActiveSSBOs );
	for ( GLuint i = 0; i < numActiveSSBOs; i++ )
	{
		GLenum props[ 1 ] = { GL_NAME_LENGTH };
		GLint res[ 1 ];
		glGetProgramResourceiv( program.handle, GL_SHADER_STORAGE_BLOCK, i, std::size( props ), props, std::size( res ), nullptr, res );

		std::string name( (GLuint)res[ 0 ] - 1, '\0' );
		glGetProgramResourceName( program.handle, GL_SHADER_STORAGE_BLOCK, i, name.capacity() + 1, nullptr, name.data() );
		
		// create ssbo

		sGPUBufferDesc ubDesc;
		ubDesc.name = name;
		ubDesc.type = WV_BUFFER_TYPE_DYNAMIC_STORAGE;
		ubDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		cGPUBuffer& buf = *createGPUBuffer( &ubDesc );

		sOpenGLBufferData* pUBData = new sOpenGLBufferData();
		buf.pPlatformData = pUBData;

		pUBData->bindingIndex = getBufferBindingIndex();
		pUBData->blockIndex = glGetProgramResourceIndex( program.handle, GL_SHADER_STORAGE_BLOCK, name.data() );
		
		WV_ASSERT_GL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, pUBData->bindingIndex, buf.handle ) );
		
		WV_ASSERT_GL( glShaderStorageBlockBinding( program.handle, pUBData->blockIndex, pUBData->bindingIndex ) );
		
		program.shaderBuffers.push_back( &buf );
	}

	return id;
#else
	return ShaderProgramID{ 0 };
#endif
}

void wv::cOpenGLGraphicsDevice::destroyProgram( ShaderProgramID _programID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( !_programID.isValid() )
		return;

	sShaderProgram& program = m_shaderPrograms.get( _programID );

	WV_ASSERT_GL( glDeleteProgram( program.handle ) );
	
	for( auto& buffer : program.shaderBuffers )
		destroyGPUBuffer( buffer );
	
	m_shaderPrograms.deallocate( _programID );
#endif
}

wv::PipelineID wv::cOpenGLGraphicsDevice::createPipeline( sPipelineDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	
	sPipelineDesc& desc = *_desc;
	PipelineID id = m_pipelines.allocate();
	sPipeline& pipeline = m_pipelines.get( id );
	
	WV_ASSERT_GL( glCreateProgramPipelines( 1, &pipeline.handle ) );
	
	if( pipeline.handle == 0 )
	{
		m_pipelines.deallocate( id );
		return PipelineID{ 0 };
	}

	if ( desc.pVertexProgram )
	{
		//sShaderProgram& vs = *m_shaderPrograms.at( *desc.pVertexProgram );
		sShaderProgram& vs = m_shaderPrograms.get( *desc.pVertexProgram );
		WV_ASSERT_GL( glUseProgramStages( pipeline.handle, GL_VERTEX_SHADER_BIT, vs.handle ) );
		pipeline.pVertexProgram   = *desc.pVertexProgram;
	}
	if ( desc.pFragmentProgram )
	{
		//sShaderProgram& fs = *m_shaderPrograms.at( *desc.pFragmentProgram );
		sShaderProgram& fs = m_shaderPrograms.get( *desc.pFragmentProgram );
		WV_ASSERT_GL( glUseProgramStages( pipeline.handle, GL_FRAGMENT_SHADER_BIT, fs.handle ) );
		pipeline.pFragmentProgram = *desc.pFragmentProgram;
	}

	return id;
#else
	return PipelineID{ 0 };
#endif
}

void wv::cOpenGLGraphicsDevice::destroyPipeline( PipelineID _pipelineID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sPipeline& pipeline = m_pipelines.get( _pipelineID );

	WV_ASSERT_GL( glDeleteProgramPipelines( 1, &pipeline.handle ) );
	
	destroyProgram( pipeline.pVertexProgram );
	destroyProgram( pipeline.pFragmentProgram );

	if( pipeline.pPlatformData )
		delete pipeline.pPlatformData;

	m_pipelines.deallocate( _pipelineID );
#endif
}

void wv::cOpenGLGraphicsDevice::bindPipeline( PipelineID _pipelineID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( m_activePipeline == _pipelineID )
		return;

	sPipeline& pipeline = m_pipelines.get( _pipelineID );
	WV_ASSERT_GL( glBindProgramPipeline( pipeline.handle ) );

	m_activePipeline = _pipelineID;
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
	
	glCreateBuffers( 1, &buffer.handle );
	
	assertGLError( "Failed to create buffer\n" );

	if ( _desc->size > 0 )
		allocateBuffer( &buffer, _desc->size );
	
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

		// only way to recreate a buffer on the gpu from what I can tell
		// no DSA way it seems?
		WV_ASSERT_GL( glBindBuffer( target, buffer.handle ) );
		// glNamedBufferData( buffer.handle, buffer.size, buffer.pData, usage );
		WV_ASSERT_GL( glBufferData( target, buffer.size, buffer.pData, usage ) );
		WV_ASSERT_GL( glBindBuffer( target, 0 ) );

		buffer.bufferedSize = buffer.size;
	}
	else
	{
		WV_ASSERT_GL( glNamedBufferSubData( buffer.handle, 0, buffer.size, buffer.pData ) );
	}

#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::allocateBuffer( cGPUBuffer* _buffer, size_t _size )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	cGPUBuffer& buffer = *_buffer;

	if ( buffer.pData )
		delete[] buffer.pData;
	
	GLenum usage = getGlBufferUsage( buffer.usage );
	buffer.pData = new uint8_t[ _size ];
	buffer.size  = _size;
	buffer.bufferedSize = buffer.size;

	WV_ASSERT_GL( glNamedBufferData( buffer.handle, _size, 0, usage ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::destroyGPUBuffer( cGPUBuffer* _buffer )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( _buffer->pPlatformData )
	{
		sOpenGLBufferData* pData = (sOpenGLBufferData*)_buffer->pPlatformData;
		if( pData->bindingIndex != 0 )
			m_usedBindingIndices.erase( pData->bindingIndex );
		
	}

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
	
	sGPUBufferDesc vbDesc;
	vbDesc.name  = "vbo";
	vbDesc.type  = WV_BUFFER_TYPE_VERTEX;
	vbDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;
	vbDesc.size  = _desc->sizeVertices;
	mesh.pVertexBuffer = createGPUBuffer( &vbDesc );
	mesh.pMaterial = _desc->pMaterial;

	uint32_t count = _desc->sizeVertices / sizeof( Vertex );
	mesh.pVertexBuffer->count = count;
	
	mesh.pVertexBuffer->buffer( (uint8_t*)_desc->vertices, _desc->sizeVertices );
	bufferData( mesh.pVertexBuffer );
	delete[] mesh.pVertexBuffer->pData;
	mesh.pVertexBuffer->pData = nullptr;

	if ( _desc->numIndices > 0 )
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_INDICES;

		sGPUBufferDesc ibDesc;
		ibDesc.name  = "ebo";
		ibDesc.type  = WV_BUFFER_TYPE_INDEX;
		ibDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;

		mesh.pIndexBuffer = createGPUBuffer( &ibDesc );
		mesh.pIndexBuffer->count = _desc->numIndices;
		
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
		delete[] mesh.pIndexBuffer->pData;
		mesh.pIndexBuffer->pData = nullptr;
	}
	else
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_VERTICES;
	}
	
	if( _desc->pParentTransform != nullptr )
		_desc->pParentTransform->addChild( &mesh.transform );
	
	if( _desc->deleteData )
	{
		if( _desc->vertices )   { delete[] _desc->vertices; }
		if( _desc->pIndices16 ) { delete[] _desc->pIndices16; }
		if( _desc->pIndices32 ) { delete[] _desc->pIndices32; }
	}

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
	sMesh& mesh = *_pMesh;
	destroyGPUBuffer( mesh.pIndexBuffer );
	destroyGPUBuffer( mesh.pVertexBuffer );

	if( mesh.pPlatformData )
		delete mesh.pPlatformData;

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

	WV_ASSERT_GL( glCreateTextures( GL_TEXTURE_2D, 1, &texture.textureObjectHandle ) );
	
	GLenum minFilter = GL_NEAREST;
	GLenum magFilter = GL_NEAREST;

	if ( desc.generateMipMaps )
	{
		switch ( desc.filtering )
		{
		case WV_TEXTURE_FILTER_NEAREST: magFilter = GL_NEAREST; minFilter = GL_NEAREST_MIPMAP_LINEAR; break;
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

	glTextureParameteri( texture.textureObjectHandle, GL_TEXTURE_MIN_FILTER, minFilter );
	glTextureParameteri( texture.textureObjectHandle, GL_TEXTURE_MAG_FILTER, magFilter );
	glTextureParameteri( texture.textureObjectHandle, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTextureParameteri( texture.textureObjectHandle, GL_TEXTURE_WRAP_T, GL_REPEAT );

	GLenum type = GL_UNSIGNED_BYTE;
	switch ( desc.format )
	{
	case wv::WV_TEXTURE_FORMAT_FLOAT: type = GL_FLOAT; break;
	case wv::WV_TEXTURE_FORMAT_INT: type = GL_INT; break;
	}

	WV_ASSERT_GL( glTextureImage2DEXT( texture.textureObjectHandle, GL_TEXTURE_2D, 0, internalFormat, desc.width, desc.height, 0, format, type, nullptr ) );
	
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
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sOpenGLTextureData* pPData = (sOpenGLTextureData*)_pTexture->pPlatformData;

	glTextureImage2DEXT( _pTexture->textureObjectHandle, GL_TEXTURE_2D, 0, pPData->internalFormat, _pTexture->width, _pTexture->height, 0, pPData->format, pPData->type, _pData );
	if ( _generateMipMaps )
	{
		WV_ASSERT_GL( glGenerateTextureMipmap( _pTexture->textureObjectHandle ) );
	}
	
	_pTexture->textureHandle = WV_ASSERT_GL( glGetTextureHandleARB( _pTexture->textureObjectHandle ) );
	
	WV_ASSERT_GL( glMakeTextureHandleResidentARB( _pTexture->textureHandle ) );
	
	_pTexture->pData = (uint8_t*)_pData;
#endif
}

void wv::cOpenGLGraphicsDevice::destroyTexture( sTexture* _pTexture )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glMakeTextureHandleNonResidentARB( _pTexture->textureHandle );

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
		WV_ASSERT_GL( glBindTextureUnit( _slot, _pTexture->textureObjectHandle ) );
	}
	else
	{
		WV_ASSERT_GL( glActiveTexture( GL_TEXTURE0 + _slot ) );
		WV_ASSERT_GL( glBindTexture( GL_TEXTURE_2D, _pTexture->textureObjectHandle ) );
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::bindVertexBuffer( sMesh* _pMesh, cPipelineResource* _pPipeline )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( !_pPipeline )
		return;

	wv::cGPUBuffer* SbVertices = _pPipeline->getShaderBuffer( "SbVertices" );

	if( SbVertices && _pMesh->pVertexBuffer )
	{
		sOpenGLBufferData* pData = (sOpenGLBufferData*)SbVertices->pPlatformData;
			
		glBindBufferRange( GL_SHADER_STORAGE_BUFFER, pData->bindingIndex, _pMesh->pVertexBuffer->handle, 0, _pMesh->pVertexBuffer->size );
	}
	

	// move?
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _pMesh->pIndexBuffer->handle );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::setFillMode( eFillMode _mode )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	switch ( _mode )
	{
	case WV_FILL_MODE_SOLID:     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  break;
	case WV_FILL_MODE_WIREFRAME: glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  break;
	case WV_FILL_MODE_POINTS:    glPolygonMode( GL_FRONT_AND_BACK, GL_POINT ); break;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::draw( sMesh* _pMesh )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sMesh& rMesh = *_pMesh;

	bindVertexBuffer( _pMesh, rMesh.pMaterial->getPipeline() );
	
	/// TODO: change GL_TRIANGLES
	if ( rMesh.drawType == WV_MESH_DRAW_TYPE_INDICES )
	{
		drawIndexed( rMesh.pIndexBuffer->count );
	}
	else
	{ 
		size_t numVertices = rMesh.pVertexBuffer->count;
		glDrawArrays( GL_TRIANGLES, 0, numVertices );
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::drawIndexed( uint32_t _numIndices )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	/// TODO: allow for other draw modes
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml#:~:text=Parameters-,mode,-Specifies%20what%20kind
	
	glDrawElements( GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0 );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cOpenGLGraphicsDevice::drawIndexedInstances( uint32_t _numIndices, uint32_t _numInstances )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glDrawElementsInstanced( GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0, _numInstances );
#endif
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

wv::Handle wv::cOpenGLGraphicsDevice::getBufferBindingIndex()
{
	wv::Handle test = 1;
	while( m_usedBindingIndices.contains( test ) )
		test++;

	m_usedBindingIndices.insert( test );

	return test;
}
