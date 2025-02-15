#ifdef WV_SUPPORT_OPENGL

#include "OpenGLGraphicsDevice.h"

#include <wv/Graphics/Texture.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Decl.h>

#include <wv/Memory/Memory.h>
#include <wv/Memory/FileSystem.h>

#include <wv/Graphics/Mesh.h>
#include <wv/Graphics/RenderTarget.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Shader/ShaderResource.h>

#include <glad/glad.h>

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_DEBUG
	#define WV_VALIDATE_GL( _func ) if( _func == nullptr ) { Debug::Print( Debug::WV_PRINT_FATAL, "Missing function '%s'\n", #_func ); }
#else
	#define WV_VALIDATE_GL( _func )
#endif

#define WV_UNIMPLEMENTED throw std::runtime_error( "UNIMPLEMENTED" )

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_DEBUG
void glMessageCallback( GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, GLchar const* _message, void const* _userData )
{
	wv::Debug::PrintLevel printLevel = [ _severity ]()
		{
			switch( _severity )
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION: return wv::Debug::WV_PRINT_INFO;
			case GL_DEBUG_SEVERITY_LOW:          return wv::Debug::WV_PRINT_DEBUG;
			case GL_DEBUG_SEVERITY_MEDIUM:       return wv::Debug::WV_PRINT_WARN;
			case GL_DEBUG_SEVERITY_HIGH:         return wv::Debug::WV_PRINT_ERROR;
			}

			return wv::Debug::WV_PRINT_INFO;
		}( );
	
	wv::Debug::Print( printLevel, "%s\n", _message );
}
#endif

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

///////////////////////////////////////////////////////////////////////////////////////

wv::GraphicsDeviceOpenGL::GraphicsDeviceOpenGL()
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::GraphicsDeviceOpenGL::initialize( sLowLevelGraphicsDesc* _desc )
{
	WV_TRACE();

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

#ifdef WV_DEBUG
	glEnable( GL_DEBUG_OUTPUT );
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
	glDebugMessageCallback( glMessageCallback, nullptr );
	glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE );
#endif
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
	//m_uniformBindingIndices.setMaxIDs( maxUniformBindings );

	int maxSSBOBindings = 0;
	glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings );
	//m_ssboBindingIndices.setMaxIDs( maxSSBOBindings );

	glCreateBuffers( 1, &drawIndirectHandle );
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::terminate()
{
	WV_TRACE();

	wv::IGraphicsDevice::terminate();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::beginRender()
{
	WV_TRACE();
	assertMainThread();

	IGraphicsDevice::beginRender();

	glBindVertexArray( m_vaoHandle );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::GraphicsDeviceOpenGL::_createRenderTarget( RenderTargetID _renderTargetID, const sRenderTargetDesc& _desc )
{
	WV_TRACE();
	assertMainThread();

	if( !_renderTargetID.is_valid() )
		_renderTargetID = m_renderTargets.emplace();

	sRenderTarget& target = m_renderTargets.at( _renderTargetID );
	
	glCreateFramebuffers( 1, &target.fbHandle );
	
	target.numTextures = _desc.numTextures;

	GLenum* drawBuffers = WV_NEW_ARR( GLenum, _desc.numTextures );
	target.pTextureIDs = WV_NEW_ARR( TextureID, _desc.numTextures );

	for ( int i = 0; i < _desc.numTextures; i++ )
	{
		_desc.pTextureDescs[ i ].width  = _desc.width;
		_desc.pTextureDescs[ i ].height = _desc.height;

		std::string texname = "buffer_tex" + std::to_string( i );

		target.pTextureIDs[ i ] = _createTexture( {}, _desc.pTextureDescs[ i ] );
		sTexture& tex = m_textures.at( target.pTextureIDs[ i ] );

		glNamedFramebufferTexture( target.fbHandle, GL_COLOR_ATTACHMENT0 + i, tex.textureObjectHandle, 0 );
		drawBuffers[ i ] = GL_COLOR_ATTACHMENT0 + i;
	}

	glCreateRenderbuffers( 1, &target.rbHandle );

	glNamedRenderbufferStorage( target.rbHandle, GL_DEPTH_COMPONENT24, _desc.width, _desc.height );
	glNamedFramebufferRenderbuffer( target.fbHandle, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, target.rbHandle );

	glNamedFramebufferDrawBuffers( target.fbHandle, _desc.numTextures, drawBuffers );

	WV_FREE_ARR( drawBuffers );
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

		_destroyRenderTarget( _renderTargetID );
		
		return RenderTargetID::InvalidID;
	}
#endif
	target.width  = _desc.width;
	target.height = _desc.height;

	return _renderTargetID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::_destroyRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();
	assertMainThread();

	sRenderTarget& rt = m_renderTargets.at( _renderTargetID );

	glDeleteFramebuffers( 1, &rt.fbHandle );
	glDeleteRenderbuffers( 1, &rt.rbHandle );

	for ( int i = 0; i < rt.numTextures; i++ )
		_destroyTexture( rt.pTextureIDs[ i ] );

	if( rt.pTextureIDs )
		WV_FREE_ARR( rt.pTextureIDs );

	m_renderTargets.erase( _renderTargetID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ShaderModuleID wv::GraphicsDeviceOpenGL::_createShaderModule( ShaderModuleID _programID, const ShaderModuleDesc& _desc )
{
	WV_TRACE();
	assertMainThread();

	if( !_programID.is_valid() )
		_programID = m_programs.emplace();

	eShaderProgramType   type   = _desc.type;
	sShaderProgramSource source = _desc.source;

	if( source.data->size == 0 )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot compile shader with null source\n" );
		return ShaderModuleID::InvalidID;
	}

	// sShaderProgram* program = new sShaderProgram();
	sProgram program{};
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

	program.handle = glCreateShaderProgramv( glType, 1, &sourcePtr );
	
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
		glGetProgramResourceName( program.handle, GL_UNIFORM_BLOCK, i, name.capacity() + 1, nullptr, name.data() );

		// create uniform buffer

		sGPUBufferDesc ubDesc;
		ubDesc.name  = name;
		ubDesc.type  = WV_BUFFER_TYPE_UNIFORM;
		ubDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		
		BufferBindingIndex index;
		if ( m_uniformBindingNameMap.contains( name ) )
		{
			index = m_uniformBindingNameMap.at( name );
			m_uniformBindingIndices.at( index )++; // increase num users
		}
		else
		{
			index = m_uniformBindingIndices.emplace();
			m_uniformBindingNameMap[ name ] = index;
		}

		wv::Handle blockIndex = glGetUniformBlockIndex( program.handle, name.c_str() );
		glGetActiveUniformBlockiv( program.handle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &ubDesc.size );
		
		GPUBufferID bufID = _createGPUBuffer( {}, ubDesc );
		sGPUBuffer& buf = m_gpuBuffers.at( bufID );
		
		buf.bindingIndex = index;
		buf.blockIndex = blockIndex;

		//WV_ASSERT_GL( glBindBufferBase( GL_UNIFORM_BUFFER, buf.bindingIndex.value, buf.handle ) );
		glUniformBlockBinding( program.handle, buf.blockIndex, buf.bindingIndex.value );
		
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

		GPUBufferID bufID = _createGPUBuffer( {}, ubDesc );
		sGPUBuffer& buf = m_gpuBuffers.at( bufID );

		buf.bindingIndex = m_ssboBindingIndices.emplace();
		buf.blockIndex = glGetProgramResourceIndex( program.handle, GL_SHADER_STORAGE_BLOCK, name.data() );
		
		glBindBufferBase( GL_SHADER_STORAGE_BUFFER, buf.bindingIndex.value, buf.handle );
		glShaderStorageBlockBinding( program.handle, buf.blockIndex, buf.bindingIndex.value );
		
		program.shaderBuffers.push_back( bufID );
	}

	m_programs.at( _programID ) = program;

	return _programID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::_destroyShaderModule( ShaderModuleID _programID )
{
	WV_TRACE();
	assertMainThread();

	if( !_programID.is_valid() )
		return;

	sProgram& program = m_programs.at( _programID );

	glDeleteProgram( program.handle );
	
	for( auto& buffer : program.shaderBuffers )
		_destroyGPUBuffer( buffer );
	
	m_programs.erase( _programID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::GraphicsDeviceOpenGL::_createPipeline( PipelineID _pipelineID, const sPipelineDesc& _desc )
{
	WV_TRACE();
	assertMainThread();

	if( !_pipelineID.is_valid() )
		_pipelineID = m_pipelines.emplace();

	sPipeline& pipeline = m_pipelines.at( _pipelineID );
	pipeline.vertexProgramID   = _desc.vertexProgramID;
	pipeline.fragmentProgramID = _desc.fragmentProgramID;

	glCreateProgramPipelines( 1, &pipeline.handle );
	
	if( pipeline.handle == 0 )
	{
		wv::Debug::Print( Debug::WV_PRINT_ERROR, "Pipeline handle is 0\n" );
		m_pipelines.erase( _pipelineID );
		return PipelineID::InvalidID;
	}

	sProgram& vs = m_programs.at( _desc.vertexProgramID );
	sProgram& fs = m_programs.at( _desc.fragmentProgramID );
	
	glUseProgramStages( pipeline.handle, GL_VERTEX_SHADER_BIT,   vs.handle );
	glUseProgramStages( pipeline.handle, GL_FRAGMENT_SHADER_BIT, fs.handle );
	
	return _pipelineID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::_destroyPipeline( PipelineID _pipelineID )
{
	WV_TRACE();
	assertMainThread();

	sPipeline& pipeline = m_pipelines.at( _pipelineID );

	glDeleteProgramPipelines( 1, &pipeline.handle );
	
	_destroyShaderModule( pipeline.vertexProgramID );
	_destroyShaderModule( pipeline.fragmentProgramID );

	if( pipeline.pPlatformData )
		WV_FREE( pipeline.pPlatformData );

	m_pipelines.erase( _pipelineID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::GraphicsDeviceOpenGL::_createGPUBuffer( GPUBufferID _bufferID, const sGPUBufferDesc& _desc )
{
	WV_TRACE();
	assertMainThread();

	if( !_bufferID.is_valid() )
		_bufferID = m_gpuBuffers.emplace();

	sGPUBuffer& buffer = m_gpuBuffers.at( _bufferID );

	buffer.type  = _desc.type;
	buffer.usage = _desc.usage;
	buffer.name  = _desc.name;

	GLenum target = getGlBufferEnum( buffer.type );
	
	glCreateBuffers( 1, &buffer.handle );
	
	GLenum usage = getGlBufferUsage( buffer.usage );
	buffer.size = _desc.size;
	
	glNamedBufferData( buffer.handle, _desc.size, 0, usage );
		
	buffer.complete = true;

	return _bufferID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::_destroyGPUBuffer( GPUBufferID _bufferID )
{
	WV_TRACE();
	assertMainThread();

	sGPUBuffer& buffer = m_gpuBuffers.at( _bufferID );

	//if( buffer.pPlatformData )
	{
		//sOpenGLBufferData* pData = (sOpenGLBufferData*)buffer.pPlatformData;
		{
			if( buffer.bindingIndex.is_valid() )
			{
				switch( buffer.type )
				{
				case WV_BUFFER_TYPE_DYNAMIC: m_ssboBindingIndices.erase( buffer.bindingIndex );    break;
				case WV_BUFFER_TYPE_UNIFORM: 
				{
					uint8_t idx = m_uniformBindingIndices.at( buffer.bindingIndex )--;
					if( idx == 0 )
						m_uniformBindingIndices.erase( buffer.bindingIndex );

					break;
				}
				}
			}
		}
		WV_FREE( buffer.pPlatformData );
		buffer.pPlatformData = nullptr;
	}

	glDeleteBuffers( 1, &buffer.handle );

	m_gpuBuffers.erase( _bufferID );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::bindBufferIndex( GPUBufferID _bufferID, int32_t _bindingIndex )
{
	assertMainThread();

	sGPUBuffer& buffer = m_gpuBuffers.at( _bufferID );
	GLenum target = getGlBufferEnum( buffer.type );

	glBindBufferBase( target, _bindingIndex, buffer.handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base )
{
	assertMainThread();

	sGPUBuffer buffer = m_gpuBuffers.at( _bufferID );
	glNamedBufferSubData( buffer.handle, _base, _size, _pData );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::GraphicsDeviceOpenGL::_createTexture( TextureID _textureID, const sTextureDesc& _desc )
{
	WV_TRACE();
	assertMainThread();

	GLenum internalFormat = GL_R8;
	GLenum format = GL_RED;

	if( !_textureID.is_valid() )
		_textureID = m_textures.emplace();

	sTexture& texture = m_textures.at( _textureID );
	texture.numChannels = _desc.numChannels;

	switch ( _desc.channels )
	{
	case wv::WV_TEXTURE_CHANNELS_R:
		format = GL_RED;
		switch ( _desc.format )
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
		switch ( _desc.format )
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
		
		switch ( _desc.format )
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
		switch ( _desc.format )
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

	glCreateTextures( GL_TEXTURE_2D, 1, &texture.textureObjectHandle );
	
	GLenum minFilter = GL_NEAREST;
	GLenum magFilter = GL_NEAREST;

	if ( _desc.generateMipMaps )
	{
		switch ( _desc.filtering )
		{
		case WV_TEXTURE_FILTER_NEAREST: magFilter = GL_NEAREST; minFilter = GL_NEAREST_MIPMAP_LINEAR; break;
		case WV_TEXTURE_FILTER_LINEAR:  magFilter = GL_LINEAR;  minFilter = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else
	{
		switch ( _desc.filtering )
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
	switch ( _desc.format )
	{
	case wv::WV_TEXTURE_FORMAT_FLOAT: type = GL_FLOAT; break;
	case wv::WV_TEXTURE_FORMAT_INT: type = GL_INT; break;
	}

	glTextureImage2DEXT( texture.textureObjectHandle, GL_TEXTURE_2D, 0, internalFormat, _desc.width, _desc.height, 0, format, type, nullptr );
	
	sPlatformTextureData* pPData = WV_NEW( sPlatformTextureData );

	pPData->filter = minFilter;
	pPData->format = format;
	pPData->internalFormat = internalFormat;
	pPData->type = type;

	texture.pPlatformData = pPData;
	texture.width  = _desc.width;
	texture.height = _desc.height;
	
	return _textureID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::_bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps )
{
	WV_TRACE();
	assertMainThread();

	sTexture& tex = m_textures.at( _textureID );

	sPlatformTextureData* pPData = tex.pPlatformData;

	glTextureImage2DEXT( tex.textureObjectHandle, GL_TEXTURE_2D, 0, pPData->internalFormat, tex.width, tex.height, 0, pPData->format, pPData->type, _pData );
	if ( _generateMipMaps )
		glGenerateTextureMipmap( tex.textureObjectHandle );
	
	tex.textureHandle = glGetTextureHandleARB( tex.textureObjectHandle );
	
	glMakeTextureHandleResidentARB( tex.textureHandle );
	
	tex.pData = (uint8_t*)_pData;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::_destroyTexture( TextureID _textureID )
{
	WV_TRACE();
	assertMainThread();

	sTexture tex = m_textures.at( _textureID );

	glMakeTextureHandleNonResidentARB( tex.textureHandle );
	glDeleteTextures( 1, &tex.textureObjectHandle );

	if( tex.pData )
	{
		WV_FREE_ARR( tex.pData );
		tex.pData = nullptr;
	}

	if( tex.pPlatformData )
	{
		WV_FREE( tex.pPlatformData );
		tex.pPlatformData = nullptr;
	}

	m_textures.erase( _textureID );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::bindTextureToSlot( TextureID _textureID, unsigned int _slot )
{
	WV_TRACE();
	assertMainThread();

	sTexture& tex = m_textures.at( _textureID );

	/// TODO: some cleaner way of checking version/supported features
	if ( m_graphicsApiVersion.major == 4 && m_graphicsApiVersion.minor >= 5 ) // if OpenGL 4.5 or higher
	{
		glBindTextureUnit( _slot, tex.textureObjectHandle );
	}
	else
	{
		glActiveTexture( GL_TEXTURE0 + _slot );
		glBindTexture( GL_TEXTURE_2D, tex.textureObjectHandle );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::setFillMode( eFillMode _mode )
{
	WV_TRACE();
	assertMainThread();

	switch ( _mode )
	{
	case WV_FILL_MODE_SOLID:     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  break;
	case WV_FILL_MODE_WIREFRAME: glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );  break;
	case WV_FILL_MODE_POINTS:    glPolygonMode( GL_FRONT_AND_BACK, GL_POINT ); break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::GraphicsDeviceOpenGL::getError( std::string* _out )
{
	WV_TRACE();
	assertMainThread();

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
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdBeginRender( CmdBufferID _cmd, RenderTargetID _renderTargetID )
{
	WV_TRACE();
	assertMainThread();

	if ( _renderTargetID.is_valid() )
	{
		sRenderTarget& rt = m_renderTargets.at( _renderTargetID );
		unsigned int handle = rt.fbHandle;

		glBindFramebuffer( GL_FRAMEBUFFER, handle );
		glViewport( 0, 0, rt.width, rt.height );
	}
	else
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdEndRender( CmdBufferID _cmd )
{
	WV_UNIMPLEMENTED;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdClearColor( CmdBufferID _cmd, float _r, float _g, float _b, float _a )
{
	WV_TRACE();
	assertMainThread();

	glClearColor( _r, _g, _b, _a );
	glClear( GL_COLOR_BUFFER_BIT );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdImageClearColor( CmdBufferID _cmd, TextureID _image, float _r, float _g, float _b, float _a )
{
	WV_UNIMPLEMENTED;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdClearDepthStencil( CmdBufferID _cmd, double _depth, uint32_t _stencil )
{
	WV_TRACE();
	assertMainThread();

	//glClearStencil( static_cast<GLint>( _stencil ) );
	glClearDepth( _depth );
	glClear( GL_DEPTH_BUFFER_BIT );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdImageClearDepthStencil( CmdBufferID _cmd, TextureID _image, double _depth, uint32_t _stencil )
{
	WV_UNIMPLEMENTED;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdBindPipeline( CmdBufferID _cmd, PipelineID _pipeline )
{
	WV_TRACE();
	assertMainThread();

	sPipeline& pipeline = m_pipelines.at( _pipeline );
	glBindProgramPipeline( pipeline.handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdImageBlit( CmdBufferID _cmd, TextureID _src, TextureID _dst )
{
	WV_UNIMPLEMENTED;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdDispatch( CmdBufferID _cmd, uint32_t _numGroupsX, uint32_t _numGroupsY, uint32_t _numGroupsZ )
{
	WV_UNIMPLEMENTED;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdViewport( CmdBufferID _cmd, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height )
{
	WV_TRACE();
	assertMainThread();

	glViewport( _x, _y, _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdCopyBuffer( CmdBufferID _cmd, GPUBufferID _src, GPUBufferID _dst, size_t _srcOffset, size_t _dstOffset, size_t _size )
{
	assertMainThread();

	sGPUBuffer rb = m_gpuBuffers.at( _src );
	sGPUBuffer wb = m_gpuBuffers.at( _dst );

	glCopyNamedBufferSubData( rb.handle, wb.handle, _srcOffset, _dstOffset, _size );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdBindVertexBuffer( CmdBufferID _cmd, GPUBufferID _vertexBuffer )
{
	WV_UNIMPLEMENTED;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdBindIndexBuffer( CmdBufferID _cmd, GPUBufferID _indexBuffer, size_t _offset, wv::DataType _type )
{
	WV_TRACE();
	assertMainThread();

	m_indexBufferDataType = _type;
	wv::sGPUBuffer& buffer = m_gpuBuffers.at( _indexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer.handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdUpdateBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _size, void* _pData )
{
	sGPUBuffer& buffer = m_gpuBuffers.at( _buffer );

	GLenum usage = getGlBufferUsage( buffer.usage );
	GLenum target = getGlBufferEnum( buffer.type );

	glNamedBufferData( buffer.handle, _size, _pData, usage );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdUpdateSubBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _offset, size_t _size, void* _pData )
{
	sGPUBuffer& buffer = m_gpuBuffers.at( _buffer );

	glNamedBufferSubData( buffer.handle, _offset, _size, _pData );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdDraw( CmdBufferID _cmd, uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance )
{
	WV_TRACE();
	assertMainThread();

	glDrawArraysInstancedBaseInstance(
		GL_TRIANGLES,
		_firstVertex,
		_vertexCount,
		_instanceCount,
		_firstInstance );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GraphicsDeviceOpenGL::cmdDrawIndexed( CmdBufferID _cmd, uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, int32_t _vertexOffset, uint32_t _firstInstance )
{
	// m_indexBufferDataType
	/// TODO:
	GLenum type = GL_UNSIGNED_INT;

	glDrawElementsInstancedBaseVertexBaseInstance(
		GL_TRIANGLES,
		_indexCount,
		type,
		nullptr,
		_instanceCount,
		_firstIndex,
		_firstInstance );
}


#endif