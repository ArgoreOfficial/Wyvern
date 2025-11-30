#include "renderer.h"

#include <wv/debug/log.h>

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
	const char* source;
	const char* type;
	const char* severity;

	switch ( _source )
	{
	case GL_DEBUG_SOURCE_API:             source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source = "WINDOW SYSTEM"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: source = "SHADER COMPILER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     source = "THIRD PARTY"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     source = "APPLICATION"; break;
	case GL_DEBUG_SOURCE_OTHER:           source = "UNKNOWN"; break;
	default: source = "UNKNOWN"; break;
	}

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

	printf( "[GLCALLBACK] %s\n  ID: %d\n  Severity: %s\n  Source: %s\n  Msg:\n    %s\n", type, _id, severity, source, _msg );
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::OpenGLRenderer::setup()
{
	if ( gladLoadGL() == 0 )
	{
		printf( "Failed to load OpenGL.\n" );
		return false;
	}
	
	printf( "Loaded Graphics Device\n" );
	printf( "  %s\n", glGetString( GL_VERSION ) );
	
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

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::shutdown()
{
	glDeleteVertexArrays( 1, &m_empty_vao );
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

void wv::OpenGLRenderer::clear( float _r, float _g, float _b, float _a )
{
	glClearColor( _r, _g, _b, _a );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::draw( int _first, uint32_t _count )
{
	glDrawArrays( GL_TRIANGLES, _first, _count );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::OpenGLRenderer::createPipeline( const char* _vert_src, const char* _frag_src )
{
	GLShaderPipeline pipeline{};
	
	pipeline.vert_module_handle = compileShaderModule( _vert_src, GL_VERTEX_SHADER );
	pipeline.frag_module_handle = compileShaderModule( _frag_src, GL_FRAGMENT_SHADER );

	if ( pipeline.vert_module_handle == 0 || pipeline.frag_module_handle == 0 )
		return ResourceID::InvalidID;
	
	pipeline.pipeline_handle = 0;
	
	/*
	GL_ASSERT(glGenProgramPipelines, 1, &pipeline.pipeline_handle );
	GL_ASSERT(glBindProgramPipeline, pipeline.pipeline_handle );
	
	if ( pipeline.pipeline_handle == 0 )
	{
		printf( "error: pipeline_handle is 0\n" );
		return ResourceID::InvalidID;
	}

	glUseProgramStages( pipeline.pipeline_handle, GL_VERTEX_SHADER_BIT, pipeline.vert_module_handle );
	glUseProgramStages( pipeline.pipeline_handle, GL_FRAGMENT_SHADER_BIT, pipeline.frag_module_handle );

	glBindProgramPipeline( 0 );
	*/

	pipeline.pipeline_handle = glCreateProgram();
	glAttachShader( pipeline.pipeline_handle, pipeline.vert_module_handle );
	glAttachShader( pipeline.pipeline_handle, pipeline.frag_module_handle );
	glLinkProgram( pipeline.pipeline_handle );
	
	int success = 0;
	char infoLog[ 512 ];
	glGetProgramiv( pipeline.pipeline_handle, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( pipeline.pipeline_handle, 512, NULL, infoLog );
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to link program\n%s\n", infoLog );
	}

	return m_pipelines.emplace( pipeline );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::destroyPipeline( ResourceID _handle )
{
	if ( !_handle.is_valid() )
		return;

	wv::GLShaderPipeline& pipeline = m_pipelines[ _handle ];
	
	/*
	glDeleteProgram( pipeline.frag_module_handle );
	glDeleteProgram( pipeline.vert_module_handle );

	glDeleteProgramPipelines( 1, &pipeline.pipeline_handle );
	*/

	glDeleteShader( pipeline.frag_module_handle );
	glDeleteShader( pipeline.vert_module_handle );

	glDeleteProgram( pipeline.pipeline_handle );

	m_pipelines.erase( _handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::bindPipeline( ResourceID _handle )
{
	if ( !_handle.is_valid() )
		return;

	wv::GLShaderPipeline& pipeline = m_pipelines[ _handle ];

	if ( pipeline.pipeline_handle == 0 )
	{
		printf( "[ERROR] Cannot bind pipeline %hi. Handle is 0\n", _handle.value );
		return;
	}

	// glBindProgramPipeline( pipeline.pipeline_handle );
	glUseProgram( pipeline.pipeline_handle );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::OpenGLRenderer::createVertexBuffer( Vertex* _data, size_t _data_size )
{
	GLVertexBuffer vbuffer{};
	
	GL_ASSERT( glGenBuffers, 1, &vbuffer.storage_buffer_handle );
	GL_ASSERT( glBindBuffer, GL_SHADER_STORAGE_BUFFER, vbuffer.storage_buffer_handle );
	GL_ASSERT( glBufferData, GL_SHADER_STORAGE_BUFFER, _data_size, _data, GL_STATIC_DRAW );
	
	vbuffer.size = _data_size;

	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );

	return m_vertex_buffers.emplace( vbuffer );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::destroyVertexBuffer( ResourceID _handle )
{
	GLVertexBuffer& vbuffer = m_vertex_buffers[ _handle ];
	glDeleteBuffers( 1, &vbuffer.storage_buffer_handle );

	m_vertex_buffers.erase( _handle );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::bindVertexBuffer( ResourceID _handle )
{
	GLVertexBuffer& vbuffer = m_vertex_buffers[ _handle ];
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, vbuffer.storage_buffer_handle );
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

void wv::OpenGLRenderer::setVSUniformMatrix4x4( ResourceID _handle, uint32_t _location, const wv::Matrix4x4f& _matrix )
{
	if ( !_handle.is_valid() )
		return;

	//glProgramUniformMatrix4fv( m_pipelines.at( _handle ).vert_module_handle, _location, 1, GL_FALSE, _matrix.m );
	glUniformMatrix4fv( _location, 1, GL_FALSE, _matrix.m );
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OpenGLRenderer::setFSUniformMatrix4x4( ResourceID _handle, uint32_t _location, const wv::Matrix4x4f& _matrix )
{
	if ( !_handle.is_valid() )
		return;
	
	//glProgramUniformMatrix4fv( m_pipelines.at( _handle ).frag_module_handle, _location, 1, GL_FALSE, _matrix.m );
	glUniformMatrix4fv( _location, 1, GL_FALSE, _matrix.m );
}

void wv::OpenGLRenderer::setVSUniformVector2f( ResourceID _pipeline, uint32_t _location, const wv::Vector2f& _vector )
{
	if ( !_pipeline.is_valid() )
		return;

	//glProgramUniform2fv( m_pipelines.at( _pipeline ).vert_module_handle, _location, 1, &_vector.x );
	glUniform2fv( _location, 1, &_vector.x );
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
