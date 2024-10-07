#include "LowLevelGraphicsOpenGL.h"

#include <wv/Graphics/Texture.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Decl.h>

#include <wv/Memory/FileSystem.h>

#include <wv/Graphics/Mesh.h>
#include <wv/Graphics/RenderTarget.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Shader/ShaderResource.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif 

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

#define WV_HARD_ASSERT 1

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
	case wv::WV_BUFFER_TYPE_DYNAMIC: return GL_SHADER_STORAGE_BUFFER; break;
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

wv::cLowLevelGraphicsOpenGL::cLowLevelGraphicsOpenGL()
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cLowLevelGraphicsOpenGL::initialize( sLowLevelGraphicsDesc* _desc )
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

	int maxUniformBindings = 0;
	glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBindings );
	m_uniformBindingIndices.setMaxIDs( maxUniformBindings );

	int maxSSBOBindings = 0;
	glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings );
	m_ssboBindingIndices.setMaxIDs( maxSSBOBindings );

	return true;
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::terminate()
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::onResize( int _width, int _height )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::setViewport( int _width, int _height )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glViewport( 0, 0, _width, _height );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::beginRender()
{
	WV_TRACE();
	
	iLowLevelGraphics::beginRender();

#ifdef WV_SUPPORT_OPENGL
	glBindVertexArray( m_vaoHandle );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::cLowLevelGraphicsOpenGL::createRenderTarget( RenderTargetID _renderTargetID, sRenderTargetDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sRenderTargetDesc& desc = *_desc;
	if( !_renderTargetID.isValid() )
		_renderTargetID = m_renderTargets.allocate();

	sRenderTarget& target = m_renderTargets.get( _renderTargetID );
	
	glCreateFramebuffers( 1, &target.fbHandle );
	
	target.numTextures = desc.numTextures;
	GLenum* drawBuffers = new GLenum[ desc.numTextures ];
	target.pTextureIDs = new TextureID[ desc.numTextures ];

	for ( int i = 0; i < desc.numTextures; i++ )
	{
		desc.pTextureDescs[ i ].width = desc.width;
		desc.pTextureDescs[ i ].height = desc.height;

		std::string texname = "buffer_tex" + std::to_string( i );

		target.pTextureIDs[ i ] = createTexture( 0, &desc.pTextureDescs[ i ] );
		sTexture& tex = m_textures.get( target.pTextureIDs[ i ] );

		glNamedFramebufferTexture( target.fbHandle, GL_COLOR_ATTACHMENT0 + i, tex.textureObjectHandle, 0 );
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

		destroyRenderTarget( _renderTargetID );
		
		return RenderTargetID::InvalidID;
	}
#endif
	target.width  = desc.width;
	target.height = desc.height;

	return _renderTargetID;
#else
	return RenderTargetID::InvalidID;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::destroyRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sRenderTarget& rt = m_renderTargets.get( _renderTargetID );

	glDeleteFramebuffers( 1, &rt.fbHandle );
	glDeleteRenderbuffers( 1, &rt.rbHandle );

	for ( int i = 0; i < rt.numTextures; i++ )
		destroyTexture( rt.pTextureIDs[ i ] );

	m_renderTargets.deallocate( _renderTargetID );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::setRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sRenderTarget& rt = m_renderTargets.get( _renderTargetID );
	unsigned int handle = rt.fbHandle;

	glBindFramebuffer( GL_FRAMEBUFFER, handle );
	glViewport( 0, 0, rt.width, rt.height );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::setClearColor( const wv::cColor& _color )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glClearColor( _color.r, _color.g, _color.b, _color.a );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::clearRenderTarget( bool _color, bool _depth )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glClear( (GL_COLOR_BUFFER_BIT * _color) | (GL_DEPTH_BUFFER_BIT * _depth) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ProgramID wv::cLowLevelGraphicsOpenGL::createProgram( ProgramID _programID, sProgramDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( !_programID.isValid() )
		_programID = m_programs.allocate();

	eShaderProgramType&   type   = _desc->type;
	sShaderProgramSource& source = _desc->source;

	if( source.data->size == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile shader with null source\n" );
		return ProgramID::InvalidID;
	}

	// sShaderProgram* program = new sShaderProgram();
	sProgram& program = m_programs.get( _programID );
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
		

		sOpenGLBufferData* pUBData = new sOpenGLBufferData();
		
		pUBData->bindingIndex = m_uniformBindingIndices.allocate();

		pUBData->blockIndex = glGetUniformBlockIndex( program.handle, name.c_str() );
		WV_ASSERT_GL( glGetActiveUniformBlockiv( program.handle, pUBData->blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &ubDesc.size ) );
		
		GPUBufferID bufID = createGPUBuffer( 0, &ubDesc );
		sGPUBuffer& buf = m_gpuBuffers.get( bufID );
		buf.pPlatformData = pUBData;

		// allocateBuffer( bufID, buf.size );
		
		WV_ASSERT_GL( glBindBufferBase( GL_UNIFORM_BUFFER, pUBData->bindingIndex.value, buf.handle ) );
		WV_ASSERT_GL( glUniformBlockBinding( program.handle, pUBData->blockIndex, pUBData->bindingIndex.value ) );
		
		program.shaderBuffers.push_back( bufID );
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
		ubDesc.type = WV_BUFFER_TYPE_DYNAMIC;
		ubDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		ubDesc.size = 0;

		GPUBufferID bufID = createGPUBuffer( 0, &ubDesc );
		sGPUBuffer& buf = m_gpuBuffers.get( bufID );

		sOpenGLBufferData* pUBData = new sOpenGLBufferData();
		buf.pPlatformData = pUBData;

		pUBData->bindingIndex = m_ssboBindingIndices.allocate();
		pUBData->blockIndex = glGetProgramResourceIndex( program.handle, GL_SHADER_STORAGE_BLOCK, name.data() );
		
		WV_ASSERT_GL( glBindBufferBase( GL_SHADER_STORAGE_BUFFER, pUBData->bindingIndex.value, buf.handle ) );
		WV_ASSERT_GL( glShaderStorageBlockBinding( program.handle, pUBData->blockIndex, pUBData->bindingIndex.value ) );
		
		program.shaderBuffers.push_back( bufID );
	}

	return _programID;
#else
	return ShaderProgramID::InvalidID;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::destroyProgram( ProgramID _programID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( !_programID.isValid() )
		return;

	sProgram& program = m_programs.get( _programID );

	WV_ASSERT_GL( glDeleteProgram( program.handle ) );
	
	for( auto& buffer : program.shaderBuffers )
		destroyGPUBuffer( buffer );
	
	m_programs.deallocate( _programID );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::cLowLevelGraphicsOpenGL::createPipeline( PipelineID _pipelineID, sPipelineDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sPipelineDesc& desc = *_desc;
	if( !_pipelineID.isValid() )
		_pipelineID = m_pipelines.allocate();

	sPipeline& pipeline = m_pipelines.get( _pipelineID );
	pipeline.vertexProgramID   = desc.vertexProgramID;
	pipeline.fragmentProgramID = desc.fragmentProgramID;

	WV_ASSERT_GL( glCreateProgramPipelines( 1, &pipeline.handle ) );
	
	if( pipeline.handle == 0 )
	{
		m_pipelines.deallocate( _pipelineID );
		return PipelineID::InvalidID;
	}

	sProgram& vs = m_programs.get( desc.vertexProgramID );
	sProgram& fs = m_programs.get( desc.fragmentProgramID );
	
	WV_ASSERT_GL( glUseProgramStages( pipeline.handle, GL_VERTEX_SHADER_BIT,   vs.handle ) );
	WV_ASSERT_GL( glUseProgramStages( pipeline.handle, GL_FRAGMENT_SHADER_BIT, fs.handle ) );
	
	return _pipelineID;
#else
	return PipelineID::InvalidID;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::destroyPipeline( PipelineID _pipelineID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sPipeline& pipeline = m_pipelines.get( _pipelineID );

	WV_ASSERT_GL( glDeleteProgramPipelines( 1, &pipeline.handle ) );
	
	destroyProgram( pipeline.vertexProgramID );
	destroyProgram( pipeline.fragmentProgramID );

	if( pipeline.pPlatformData )
		delete pipeline.pPlatformData;

	m_pipelines.deallocate( _pipelineID );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::bindPipeline( PipelineID _pipelineID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sPipeline& pipeline = m_pipelines.get( _pipelineID );
	WV_ASSERT_GL( glBindProgramPipeline( pipeline.handle ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::cLowLevelGraphicsOpenGL::createGPUBuffer( GPUBufferID _bufferID, sGPUBufferDesc* _desc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( !_bufferID.isValid() )
		_bufferID = m_gpuBuffers.allocate();

	sGPUBuffer& buffer = m_gpuBuffers.get( _bufferID );

	buffer.type  = _desc->type;
	buffer.usage = _desc->usage;
	buffer.name  = _desc->name;

	GLenum target = getGlBufferEnum( buffer.type );
	
	WV_ASSERT_GL( glCreateBuffers( 1, &buffer.handle ) );
	
	GLenum usage = getGlBufferUsage( buffer.usage );
	buffer.size = _desc->size;
	
	WV_ASSERT_GL( glNamedBufferData( buffer.handle, _desc->size, 0, usage ) );
		
	buffer.complete = true;

	return _bufferID;
#else 
	return GPUBufferID::InvalidID;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::bufferData( GPUBufferID _bufferID, void* _pData, size_t _size )
{
	sGPUBuffer& buffer = m_gpuBuffers.get( _bufferID );

	GLenum usage  = getGlBufferUsage( buffer.usage );
	GLenum target = getGlBufferEnum ( buffer.type );

	WV_ASSERT_GL( glBindBuffer( target, buffer.handle ) );
	WV_ASSERT_GL( glBufferData( target, _size, _pData, usage ) );
	WV_ASSERT_GL( glBindBuffer( target, 0 ) );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base )
{
	sGPUBuffer& buffer = m_gpuBuffers.get( _bufferID );
	WV_ASSERT_GL( glNamedBufferSubData( buffer.handle, _base, _size, _pData ); );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::destroyGPUBuffer( GPUBufferID _bufferID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sGPUBuffer& buffer = m_gpuBuffers.get( _bufferID );

	if( buffer.pPlatformData )
	{
		sOpenGLBufferData* pData = (sOpenGLBufferData*)buffer.pPlatformData;
		{
			if( pData->bindingIndex.isValid() )
			{
				switch( buffer.type )
				{
				case WV_BUFFER_TYPE_DYNAMIC: m_ssboBindingIndices.deallocate( pData->bindingIndex );    break;
				case WV_BUFFER_TYPE_UNIFORM: m_uniformBindingIndices.deallocate( pData->bindingIndex ); break;
				}
			}
		}
		delete buffer.pPlatformData;
		buffer.pPlatformData = nullptr;
	}

	glDeleteBuffers( 1, &buffer.handle );

	m_gpuBuffers.deallocate( _bufferID );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::cLowLevelGraphicsOpenGL::createTexture( TextureID _textureID, sTextureDesc* _pDesc )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	GLenum internalFormat = GL_R8;
	GLenum format = GL_RED;

	if( !_textureID.isValid() )
		_textureID = m_textures.allocate();

	sTextureDesc& desc = *_pDesc;
	sTexture& texture = m_textures.get( _textureID );
	texture.numChannels = desc.numChannels;

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
	
	return _textureID;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sTexture& tex = m_textures.get( _textureID );

	sOpenGLTextureData* pPData = (sOpenGLTextureData*)tex.pPlatformData;

	glTextureImage2DEXT( tex.textureObjectHandle, GL_TEXTURE_2D, 0, pPData->internalFormat, tex.width, tex.height, 0, pPData->format, pPData->type, _pData );
	if ( _generateMipMaps )
	{
		WV_ASSERT_GL( glGenerateTextureMipmap( tex.textureObjectHandle ) );
	}
	
	tex.textureHandle = WV_ASSERT_GL( glGetTextureHandleARB( tex.textureObjectHandle ) );
	
	WV_ASSERT_GL( glMakeTextureHandleResidentARB( tex.textureHandle ) );
	
	tex.pData = (uint8_t*)_pData;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::destroyTexture( TextureID _textureID )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sTexture& tex = m_textures.get( _textureID );

	glMakeTextureHandleNonResidentARB( tex.textureHandle );

	glDeleteTextures( 1, &tex.textureObjectHandle );
	if( tex.pData )
	{
		delete[] tex.pData;
		tex.pData = nullptr;
	}

	if( tex.pPlatformData )
	{
		delete tex.pPlatformData;
		tex.pPlatformData = nullptr;
	}

	m_textures.deallocate( _textureID );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::bindTextureToSlot( TextureID _textureID, unsigned int _slot )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	sTexture& tex = m_textures.get( _textureID );

	/// TODO: some cleaner way of checking version/supported features
	if ( m_graphicsApiVersion.major == 4 && m_graphicsApiVersion.minor >= 5 ) // if OpenGL 4.5 or higher
	{
		WV_ASSERT_GL( glBindTextureUnit( _slot, tex.textureObjectHandle ) );
	}
	else
	{
		WV_ASSERT_GL( glActiveTexture( GL_TEXTURE0 + _slot ) );
		WV_ASSERT_GL( glBindTexture( GL_TEXTURE_2D, tex.textureObjectHandle ) );
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::bindVertexBuffer( MeshID _meshID, cShaderResource* _pShader )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	if( !_pShader )
		return;

	wv::GPUBufferID SbVerticesID = _pShader->getShaderBuffer( "SbVertices" );
	sMesh& mesh = m_meshes.get( _meshID );

	if( SbVerticesID.isValid() && mesh.vertexBufferID.isValid() )
	{
		wv::sGPUBuffer& SbVertices = m_gpuBuffers.get( SbVerticesID );
		sOpenGLBufferData* pData = (sOpenGLBufferData*)SbVertices.pPlatformData;
	
		sGPUBuffer& vbuffer = m_gpuBuffers.get( mesh.vertexBufferID );
		glBindBufferRange( GL_SHADER_STORAGE_BUFFER, pData->bindingIndex.value, vbuffer.handle, 0, vbuffer.size );
	}
	
	// move?
	sGPUBuffer& ibuffer = m_gpuBuffers.get( mesh.indexBufferID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibuffer.handle );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::setFillMode( eFillMode _mode )
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

void wv::cLowLevelGraphicsOpenGL::draw( uint32_t _firstVertex, uint32_t _numVertices )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glDrawArrays( GL_TRIANGLES, _firstVertex, _numVertices );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::drawIndexed( uint32_t _numIndices )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	/// TODO: allow for other draw modes
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml#:~:text=Parameters-,mode,-Specifies%20what%20kind
	
	glDrawElements( GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0 );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cLowLevelGraphicsOpenGL::drawIndexedInstanced( uint32_t _numIndices, uint32_t _numInstances )
{
	WV_TRACE();

#ifdef WV_SUPPORT_OPENGL
	glDrawElementsInstanced( GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0, _numInstances );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cLowLevelGraphicsOpenGL::getError( std::string* _out )
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