#include "renderer.h"

#include <wv/debug/log.h>

#include <wv/entity/world.h>
#include <wv/entity/world_sector.h>
#include <wv/graphics/systems/render_world_system.h>
#include <wv/graphics/components/mesh_component.h>
#include <wv/graphics/viewport.h>

#include <wv/camera/systems/camera_manager_system.h>
#include <wv/camera/view_volume.h>
#include <wv/camera/components/camera_component.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////

#define GL_ASSERT( _func, ... ) \
[&](){ \
	if(_func == nullptr) { \
		printf("%s is nullptr\n", #_func); \
		throw std::runtime_error("err"); \
	} return _func( __VA_ARGS__ ); }()

/*
#define GL_ASSERT( _func, ... ) \
if(_func == nullptr) \
{ printf("%s is nullptr\n", #_func); throw std::runtime_error( "function is nullptr"  ); } \
_func( __VA_ARGS__ )
*/

///////////////////////////////////////////////////////////////////////////////////////

void APIENTRY debugMessageCallback( GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, const GLchar* _msg, const void* _data )
{
	const char* type;
	const char* severity;

	switch ( _type )
	{
	case GL_DEBUG_TYPE_ERROR:               type = "ERROR"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "DEPRECATED BEHAVIOR"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type = "UDEFINED BEHAVIOR"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         type = "PORTABILITY"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         type = "PERFORMANCE"; break;
	case GL_DEBUG_TYPE_OTHER:               type = "OTHER"; break;
	case GL_DEBUG_TYPE_MARKER:              type = "MARKER"; break;
	default: type = "UNKNOWN"; break;
	}

	switch ( _severity )
	{
	case GL_DEBUG_SEVERITY_HIGH:         severity = "HIGH"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       severity = "MEDIUM"; break;
	case GL_DEBUG_SEVERITY_LOW:          severity = "LOW"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severity = "NOTIFICATION"; break;
	default: severity = "UNKNOWN"; break;
	}
	
	wv::Debug::PrintLevel level = wv::Debug::WV_PRINT_DEBUG;
	
	if ( _type == GL_DEBUG_TYPE_ERROR )
		level = wv::Debug::WV_PRINT_ERROR;
	else if( _type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR )
		level = wv::Debug::WV_PRINT_FATAL;
	else
	{
		switch ( _severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:         level = wv::Debug::WV_PRINT_WARN; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       level = wv::Debug::WV_PRINT_WARN; break;
		case GL_DEBUG_SEVERITY_LOW:          level = wv::Debug::WV_PRINT_WARN; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: level = wv::Debug::WV_PRINT_DEBUG; break;
		//default: severity = "UNKNOWN"; break;
		}
	}

	wv::Debug::Print( level, "%s\n", _msg );

	//if ( _severity != GL_DEBUG_SEVERITY_NOTIFICATION && _severity != GL_DEBUG_SEVERITY_LOW )
	//	printf( "[GLCALLBACK] %s\n  ID: %d\n  Severity: %s\n  Source: %s\n  Msg:\n    %s\n", type, _id, severity, source, _msg );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::OpenGLRenderer::setup()
{
	if ( gladLoadGL() == 0 )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_FATAL, "Failed to load OpenGL.\n" );
		return false;
	}
	
	wv::Debug::Print( "OpenGL Loaded. %s\n", glGetString( GL_VERSION ) );
	
	// debug output
	if ( glDebugMessageCallback != nullptr )
	{
		GL_ASSERT( glEnable, GL_DEBUG_OUTPUT );
		GL_ASSERT( glEnable, GL_DEBUG_OUTPUT_SYNCHRONOUS );
		GL_ASSERT( glDebugMessageCallback, debugMessageCallback, nullptr );
		GL_ASSERT( glDebugMessageControl, GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
	}

	// create and bind empty VAO permanently
	GL_ASSERT( glGenVertexArrays, 1, &m_empty_vao );
	GL_ASSERT( glBindVertexArray, m_empty_vao );
	
	// depth
	GL_ASSERT( glEnable, GL_DEPTH_TEST );
	GL_ASSERT( glDepthFunc, GL_LESS );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CW );

	// shader blocks
	glGenBuffers( 1, &m_uboSceneDataBlock );
	glBindBuffer( GL_UNIFORM_BUFFER, m_uboSceneDataBlock );
	glBufferData( GL_UNIFORM_BUFFER, sizeof( SceneData ), NULL, GL_STATIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	
	glBindBufferBase( GL_UNIFORM_BUFFER, m_sceneDataBindPoint, m_uboSceneDataBlock );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::shutdown()
{
	glDeleteVertexArrays( 1, &m_empty_vao );
	glDeleteBuffers( 1, &m_uboSceneDataBlock );

	// Debug Objects
	
	destroyMaterial( m_debugLineMaterial );
	destroyStorageBuffer( m_debugLineVertexBuffer );
	destroyStorageBuffer( m_debugLineMaterialBuffer );

	// Final memory leak check

	if ( m_renderMeshes.count()    > 0 ) WV_LOG_ERROR( "%llu render meshes remaining\n",    m_renderMeshes.count() );
	if ( m_renderMaterials.count() > 0 ) WV_LOG_ERROR( "%llu render materials remaining\n", m_renderMaterials.count() );
	if ( m_pipelines.count()       > 0 ) WV_LOG_ERROR( "%llu pipelines remaining\n",        m_pipelines.count() );
	if ( m_textures.count()        > 0 ) WV_LOG_ERROR( "%llu textures remaining\n",         m_textures.count() );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::setupDebug( const char* _vertexShaderSource, const char* _fragmentShaderSource )
{
	// Create debug rendering objects

	m_debugLineVertexBuffer   = createStorageBuffer( nullptr, sizeof( wv::Vector3f ) * 64 );
	m_debugLineMaterialBuffer = createStorageBuffer( nullptr, sizeof( MaterialData ) );
	
	m_debugLineMaterial = createMaterial();
	setMaterialVertexShader( m_debugLineMaterial, _vertexShaderSource );
	setMaterialFragmentShader( m_debugLineMaterial, _fragmentShaderSource );
	finalizeMaterial( m_debugLineMaterial );

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::prepare( uint32_t _width, uint32_t _height )
{
	glViewport( 0, 0, _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::finalize()
{
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::clearColor( float _r, float _g, float _b, float _a )
{
	glClearColor( _r, _g, _b, _a );
	glClear( GL_COLOR_BUFFER_BIT );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::clearDepth( double _depth )
{
	glClearDepth( _depth );
	glClear( GL_DEPTH_BUFFER_BIT );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::OpenGLRenderer::createMaterial()
{
	GLRenderMaterial material{};
	material.shaderProgram = glCreateProgram();

	return m_renderMaterials.emplace( material );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::destroyMaterial( ResourceID _handle )
{
	if ( !_handle.isValid() )
		return;

	GLRenderMaterial& material = m_renderMaterials.at( _handle );
	
	if ( material.shaderProgram != 0 ) glDeleteProgram( material.shaderProgram );
	if ( material.fragModule != 0 ) glDeleteShader( material.fragModule );
	if ( material.vertModule != 0 ) glDeleteShader( material.vertModule );

	m_renderMaterials.erase( _handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::setMaterialVertexShader( ResourceID _handle, const char* _src )
{
	GLRenderMaterial& material = m_renderMaterials.at( _handle );
	material.vertModule = compileShaderModule( _src, GL_VERTEX_SHADER );

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::setMaterialFragmentShader( ResourceID _handle, const char* _src )
{
	GLRenderMaterial& material = m_renderMaterials.at( _handle );
	material.fragModule = compileShaderModule( _src, GL_FRAGMENT_SHADER );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::finalizeMaterial( ResourceID _handle )
{
	GLRenderMaterial& material = m_renderMaterials.at( _handle );

	if ( material.vertModule != 0 ) glAttachShader( material.shaderProgram, material.vertModule );
	if ( material.fragModule != 0 ) glAttachShader( material.shaderProgram, material.fragModule );

	glLinkProgram( material.shaderProgram );

	int success = 0;
	char infoLog[ 512 ];
	glGetProgramiv( material.shaderProgram, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( material.shaderProgram, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to link program\n%s\n", infoLog );

		return;
	}

	material.sceneDataSlot    = glGetUniformBlockIndex( material.shaderProgram, "ubo_sceneData" );
	material.materialDataSlot = glGetUniformBlockIndex( material.shaderProgram, "ubo_materialData" );
	
	// binds shaderProgram uniform slot to block points
	if( material.sceneDataSlot    != GL_INVALID_INDEX ) glUniformBlockBinding( material.shaderProgram, material.sceneDataSlot,    m_sceneDataBindPoint );
	if( material.materialDataSlot != GL_INVALID_INDEX ) glUniformBlockBinding( material.shaderProgram, material.materialDataSlot, m_materialDataBindPoint );

}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::OpenGLRenderer::createRenderMesh( wv::Vector3f* _positions, size_t _numPositions, void* _extraVertexData, size_t _sizeExtraVertexData )
{
	GLRenderMesh mesh{};

	mesh.numVertices = _numPositions;
	mesh.positionBuffer = createStorageBuffer( (void*)_positions, sizeof( wv::Vector3f ) * _numPositions );
	
	if ( mesh.positionBuffer.handle == 0 )
		return {};

	if ( _extraVertexData )
	{
		mesh.hasExtraVertexData = true;
		mesh.extraVertexDataBuffer = createStorageBuffer( _extraVertexData, _sizeExtraVertexData );

		if ( mesh.extraVertexDataBuffer.handle == 0 )
			return {};
	}

	return m_renderMeshes.emplace( mesh );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::OpenGLRenderer::createRenderMesh( wv::Vector3f* _positions, size_t _numPositions, uint16_t* _indices, size_t _numIndices, void* _extraVertexData, size_t _sizeExtraVertexData )
{
	wv::ResourceID meshID = createRenderMesh( _positions, _numPositions, _extraVertexData, _sizeExtraVertexData );
	if ( !meshID.isValid() )
		return meshID;

	if ( _indices && _numIndices > 0 )
	{
		GLRenderMesh& mesh = m_renderMeshes[ meshID ];
		mesh.numIndices = _numIndices;
		glCreateBuffers( 1, &mesh.indexBuffer );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint16_t ) * _numIndices, _indices, GL_STATIC_DRAW );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		//glNamedBufferData( mesh.indexBuffer, sizeof( uint16_t ) * _numIndices, _indices, GL_STATIC_DRAW );
	}

	return meshID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::destroyRenderMesh( ResourceID _handle )
{
	if ( !_handle.isValid() )
		return;

	GLRenderMesh& mesh = m_renderMeshes.at( _handle );
	destroyStorageBuffer( mesh.positionBuffer );
	
	if( mesh.hasExtraVertexData )
		destroyStorageBuffer( mesh.extraVertexDataBuffer );

	m_renderMeshes.erase( _handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::setRenderMeshMaterial( ResourceID _meshHandle, ResourceID _materialHandle )
{
	wv::GLRenderMesh& mesh = m_renderMeshes.at( _meshHandle );
	wv::GLRenderMaterial& material = m_renderMaterials.at( _materialHandle );
	
	mesh.material = _materialHandle;
	mesh.materialDataBuffer.size = sizeof( MaterialData );

	// create buffer for material data
	glGenBuffers( 1, &mesh.materialDataBuffer.handle );
	glBindBuffer( GL_UNIFORM_BUFFER, mesh.materialDataBuffer.handle );
	glBufferData( GL_UNIFORM_BUFFER, mesh.materialDataBuffer.size, NULL, GL_STATIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );

	// bind uniform slot (in the shader) to dedicated material bind point
	glUniformBlockBinding( material.shaderProgram, material.materialDataSlot, m_materialDataBindPoint );

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::drawRenderBucket( const RenderBucket& _bucket )
{
	for ( size_t i = 0; i < _bucket.meshes.size(); i++ )
	{
		ResourceID meshHandle = _bucket.meshes[ i ];
		if ( !meshHandle.isValid() )
			continue;
		wv::GLRenderMesh& mesh = m_renderMeshes.at( meshHandle );
		
		if ( !mesh.material.isValid() || mesh.materialDataBuffer.handle == 0 )
			continue;

		wv::GLRenderMaterial& material = m_renderMaterials.at( mesh.material );
		
		glUseProgram( material.shaderProgram );

		// Vertex buffers
		glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, mesh.positionBuffer.handle );
		if ( mesh.hasExtraVertexData )
			glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 1, mesh.extraVertexDataBuffer.handle );

		// Material buffers
		glBindBufferBase( GL_UNIFORM_BUFFER, m_materialDataBindPoint, mesh.materialDataBuffer.handle );
		
		MaterialData materialDataTest{};
		materialDataTest.model = _bucket.modelMatrices[ i ];
		glNamedBufferSubData( mesh.materialDataBuffer.handle, 0, sizeof( MaterialData ), &materialDataTest );

		if ( mesh.numIndices > 0 && mesh.indexBuffer != 0 )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer );
			glDrawElements( GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_SHORT, nullptr );
		}
		else
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			glDrawArrays( GL_TRIANGLES, 0, static_cast<GLsizei>( mesh.numVertices ) );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::drawDebugLines( const std::vector<wv::Line3f>& _lines )
{
	// glNamedBufferSubData( m_uboSceneDataBlock, 0, sizeof( SceneData ), &_renderView.sceneData );

	if ( !m_debugLineMaterial.isValid() || m_debugLineVertexBuffer.handle == 0 || m_debugLineMaterialBuffer.handle == 0 )
		return;

	if ( _lines.size() == 0 )
		return;

	wv::GLRenderMaterial& material = m_renderMaterials.at( m_debugLineMaterial );
	glUseProgram( material.shaderProgram );

	std::vector<wv::Vector3f> positions;
	for ( size_t i = 0; i < _lines.size(); i++ )
	{
		positions.push_back( _lines[ i ].a );
		positions.push_back( _lines[ i ].b );
	}
	uploadStorageBuffer( m_debugLineVertexBuffer, positions.data(), positions.size() * sizeof( wv::Vector3f ) );

	// Vertex buffer
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, m_debugLineVertexBuffer.handle );
	
	// Material buffer
	glBindBufferBase( GL_UNIFORM_BUFFER, m_materialDataBindPoint, m_debugLineMaterialBuffer.handle );

	MaterialData materialDataTest{};
	materialDataTest.model = wv::Matrix4x4f::identity( 1.0f );
	glNamedBufferSubData( m_debugLineMaterialBuffer.handle, 0, sizeof( MaterialData ), &materialDataTest );

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glDrawArrays( GL_LINES, 0, static_cast<GLsizei>( _lines.size() * 2 ) );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); // reset back
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::renderWorld( World* _world )
{
	WV_ASSERT( _world == nullptr );

	RenderWorldSystem* worldRenderSystem = _world->getWorldSystem<RenderWorldSystem>();
	WV_ASSERT( worldRenderSystem == nullptr );

	Viewport* viewport = _world->getViewport();
	WV_ASSERT( viewport == nullptr );
	WV_ASSERT( viewport->getViewVolume() == nullptr );
	
	SceneData sceneData;
	sceneData.viewProj = viewport->getViewVolume()->getViewProjMatrix();

	glNamedBufferSubData( m_uboSceneDataBlock, 0, sizeof( SceneData ), &sceneData );

	const std::vector<SectorRenderBucket> renderBuckets = worldRenderSystem->getRenderBuckets();
	for ( auto bucket : renderBuckets )
	{
		for ( size_t i = 0; i < bucket.renderMeshes.size(); i++ )
		{
			ResourceID meshHandle = bucket.renderMeshes[ i ].meshID;
			if ( !meshHandle.isValid() )
				continue;
			wv::GLRenderMesh& mesh = m_renderMeshes.at( meshHandle );

			if ( !mesh.material.isValid() || mesh.materialDataBuffer.handle == 0 )
				continue;

			wv::GLRenderMaterial& material = m_renderMaterials.at( mesh.material );

			glUseProgram( material.shaderProgram );

			// Vertex buffers
			glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, mesh.positionBuffer.handle );
			if ( mesh.hasExtraVertexData )
				glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 1, mesh.extraVertexDataBuffer.handle );

			// Material buffers
			glBindBufferBase( GL_UNIFORM_BUFFER, m_materialDataBindPoint, mesh.materialDataBuffer.handle );

			MaterialData materialDataTest{};
			materialDataTest.model = bucket.matrices[ i ];
			glNamedBufferSubData( mesh.materialDataBuffer.handle, 0, sizeof( MaterialData ), &materialDataTest );

			if ( mesh.numIndices > 0 && mesh.indexBuffer != 0 )
			{
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer );
				glDrawElements( GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_SHORT, nullptr );
			}
			else
			{
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
				glDrawArrays( GL_TRIANGLES, 0, static_cast<GLsizei>( mesh.numVertices ) );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GLStorageBuffer wv::OpenGLRenderer::createStorageBuffer( void* _data, size_t _dataSize )
{
	GLStorageBuffer vbuffer{};
	
	GL_ASSERT( glGenBuffers, 1, &vbuffer.handle );
	GL_ASSERT( glBindBuffer, GL_SHADER_STORAGE_BUFFER, vbuffer.handle );
	GL_ASSERT( glBufferData, GL_SHADER_STORAGE_BUFFER, _dataSize, _data, GL_STATIC_COPY );
	
	vbuffer.size = _dataSize;

	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );

	return vbuffer;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::destroyStorageBuffer( const GLStorageBuffer& _buffer )
{
	glDeleteBuffers( 1, &_buffer.handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::bindStorageBufferToSlot( const GLStorageBuffer& _buffer, int _slot )
{
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, _slot, _buffer.handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::uploadStorageBuffer( const GLStorageBuffer& _buffer, void* _data, size_t _dataSize, size_t _offset )
{
	if ( _dataSize == 0 )
		return;

	size_t requiredSize = _offset + _dataSize;
	if ( _buffer.size < requiredSize )
		glNamedBufferData( _buffer.handle, requiredSize, nullptr, GL_STATIC_COPY ); // recreate buffer

	if( _data )
		glNamedBufferSubData( _buffer.handle, _offset, _dataSize, _data );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::OpenGLRenderer::createTexture( unsigned char* _data, uint32_t _width, uint32_t _height, uint32_t _channels, TextureFormat _format, bool _generate_mips, TextureFiltering _filtering )
{
	GLenum internal_format = GL_R8;
	GLenum format = GL_RED;

	GLTexture texture{};
	texture.num_channels = _channels;
	
	switch ( _channels )
	{
	case wv::WV_TEXTURE_CHANNELS_R:
		format = GL_RED;
		switch ( _format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internal_format = GL_R8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internal_format = GL_R32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internal_format = GL_R32I;
			format = GL_RED_INTEGER;
			break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RG:
		format = GL_RG;
		switch ( _format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internal_format = GL_RG8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internal_format = GL_RG32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internal_format = GL_RG32I;
			format = GL_RG_INTEGER;
			break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RGB:
		format = GL_RGB;

		switch ( _format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internal_format = GL_RGB8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internal_format = GL_RGB32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internal_format = GL_RGB32I;
			format = GL_RGB_INTEGER;
			break;
		}
		break;
	case wv::WV_TEXTURE_CHANNELS_RGBA:
		format = GL_RGBA;
		switch ( _format )
		{
		case wv::WV_TEXTURE_FORMAT_BYTE: internal_format = GL_RGBA8; break;
		case wv::WV_TEXTURE_FORMAT_FLOAT: internal_format = GL_RGBA32F; break;
		case wv::WV_TEXTURE_FORMAT_INT:
			internal_format = GL_RGBA32I;
			format = GL_RGBA_INTEGER;
			break;
		}
		break;
	}

	//glCreateTextures( GL_TEXTURE_2D, 1, &texture.texture_handle );
	GL_ASSERT( glGenTextures, 1, &texture.texture_handle );
	GL_ASSERT( glBindTexture, GL_TEXTURE_2D, texture.texture_handle );

	GLenum min_filter = GL_NEAREST;
	GLenum mag_filter = GL_NEAREST;

	if ( _generate_mips )
	{
		switch ( _filtering )
		{
		case WV_TEXTURE_FILTER_NEAREST: mag_filter = GL_NEAREST; min_filter = GL_NEAREST_MIPMAP_LINEAR; break;
		case WV_TEXTURE_FILTER_LINEAR:  mag_filter = GL_LINEAR;  min_filter = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else
	{
		switch ( _filtering )
		{
		case WV_TEXTURE_FILTER_NEAREST: mag_filter = GL_NEAREST; min_filter = GL_NEAREST; break;
		case WV_TEXTURE_FILTER_LINEAR:  mag_filter = GL_LINEAR;  min_filter = GL_LINEAR; break;
		}
	}
	
	GL_ASSERT( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter );
	GL_ASSERT( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter );
	GL_ASSERT( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	GL_ASSERT( glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	

	GLenum type = GL_UNSIGNED_BYTE;
	switch ( _format )
	{
	case wv::WV_TEXTURE_FORMAT_FLOAT: type = GL_FLOAT; break;
	case wv::WV_TEXTURE_FORMAT_INT:   type = GL_INT; break;
	}

	GL_ASSERT( glTexImage2D, GL_TEXTURE_2D, 0, internal_format, _width, _height, 0, format, type, _data );;
	
	if ( _generate_mips )
	{
		GL_ASSERT( glGenerateTextureMipmap, texture.texture_handle );
	}

	/*
	PlatformTextureData* pPData = WV_NEW( PlatformTextureData );
	texture.filter = min_filter;
	texture.format = format;
	texture.internal_format = internal_format;
	texture.type = type;
	*/

	//texture.pPlatformData = pPData;
	texture.width  = _width;
	texture.height = _height;

	GL_ASSERT( glBindTexture, GL_TEXTURE_2D, 0 );

	return m_textures.emplace( texture );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::destroyTexture( ResourceID _handle )
{
	GLTexture& texture = m_textures[ _handle ];
	glDeleteTextures( 1, &texture.texture_handle );

	m_textures.erase( _handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::bindTexture( ResourceID _handle, uint32_t _slot )
{
	GLTexture& texture = m_textures[ _handle ];

	glActiveTexture( GL_TEXTURE0 + _slot );
	glBindTexture( GL_TEXTURE_2D, texture.texture_handle );

}

wv::Vector2i wv::OpenGLRenderer::getTextureSize( ResourceID _handle )
{
	return { 
		(int)m_textures[ _handle ].width, 
		(int)m_textures[ _handle ].height 
	};
}

///////////////////////////////////////////////////////////////////////////////////////

GLuint createShaderProgram( GLenum type, GLsizei count, const char** strings )
{
	const GLuint shader = glCreateShader( type );
	if ( shader )
	{
		GL_ASSERT( glShaderSource, shader, count, strings, NULL );
		GL_ASSERT( glCompileShader, shader );
		const GLuint program = glCreateProgram();
		if ( program )
		{
			GLint compiled = GL_FALSE;
			GL_ASSERT( glGetShaderiv, shader, GL_COMPILE_STATUS, &compiled );
			GL_ASSERT( glProgramParameteri, program, GL_PROGRAM_SEPARABLE, GL_TRUE );
			if ( compiled )
			{
				GL_ASSERT( glAttachShader, program, shader );
				GL_ASSERT( glLinkProgram, program );
				GL_ASSERT( glDetachShader, program, shader );
			}
			/* append-shader-info-log-to-program-info-log */
		}
		GL_ASSERT( glDeleteShader, shader );
		return program;
	}
	else
	{
		return 0;
	}
}

GLuint wv::OpenGLRenderer::compileShaderModule( const char* _source, GLenum _type )
{
	if ( _source == nullptr )
	{
		printf( "Cannot compile shader with null source\n" );
		return 0;
	}

	if ( _type != GL_VERTEX_SHADER && _type != GL_FRAGMENT_SHADER )
	{
		printf( "Invalid shader module type\n" );
		return 0;
	}

	const GLuint shader = glCreateShader( _type );
	if ( !shader )
		return 0;

	GL_ASSERT( glShaderSource, shader, 1, &_source, NULL );
	GL_ASSERT( glCompileShader, shader );

	int  success;
	char infoLog[ 512 ];
	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( shader, 512, NULL, infoLog );
		wv::Debug::Print( Debug::WV_PRINT_ERROR, "Shader compilation failed\n %s\n", infoLog );
	}

	return shader;
}
