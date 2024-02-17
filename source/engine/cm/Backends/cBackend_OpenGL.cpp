#include "cBackend_OpenGL.h"

#include <glad/glad.h>

#include <cm/Core/cWindow.h>
#include <cm/Framework/cVertexLayout.h>

#include <stdio.h>
#include <string>

static GLuint getShaderType_OpenGL( cm::Shader::eShaderType _type )
{
	using namespace cm;

	switch ( _type )
	{
	case Shader::ShaderType_Vertex:   return GL_VERTEX_SHADER;   break;
	case Shader::ShaderType_Fragment: return GL_FRAGMENT_SHADER; break;
	case Shader::ShaderType_Geometry: return GL_GEOMETRY_SHADER; break;
	}

	return GL_NONE;
}

static GLuint getBufferTarget_OpenGL( cm::eBufferType _type )
{
	using namespace cm;

	switch ( _type )
	{
	case BufferType_Vertex:  return GL_ARRAY_BUFFER;         break;
	case BufferType_Index:   return GL_ELEMENT_ARRAY_BUFFER; break;
	case BufferType_Uniform: return GL_UNIFORM_BUFFER;       break;
	}

	return GL_BUFFER;
}

static GLuint getPrimitive_OpenGL( cm::eDrawMode _mode )
{
	using namespace cm;

	switch ( _mode )
	{
	case DrawMode_Lines:     return GL_LINES;      break;
	case DrawMode_LineLoop:  return GL_LINE_LOOP;  break;
	case DrawMode_LineStrip: return GL_LINE_STRIP; break;
	case DrawMode_Points:    return GL_POINTS;     break;
	case DrawMode_Triangle:  return GL_TRIANGLES;  break;
	}

	return GL_LINES;
}

static int getShaderType( GLenum _type )
{
	switch ( _type )
	{
	case GL_SAMPLER_1D:   return cm::Shader::ShaderUniformType_Sampler1D;   break;
	case GL_SAMPLER_2D:   return cm::Shader::ShaderUniformType_Sampler2D;   break;
	case GL_SAMPLER_3D:   return cm::Shader::ShaderUniformType_Sampler3D;   break;
	case GL_SAMPLER_CUBE: return cm::Shader::ShaderUniformType_SamplerCube; break;
	}

	return cm::Shader::ShaderUniformType_Other;
}

cm::cBackend_OpenGL::cBackend_OpenGL()
{

}

cm::cBackend_OpenGL::~cBackend_OpenGL()
{

}

void cm::cBackend_OpenGL::create( cWindow& _window )
{
	if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
	{
		fprintf( stderr, "Failed to initialize GLAD\n" );
		return;
	}
	glViewport( 0, 0, _window.getWidth(), _window.getHeight() );

	const GLubyte* renderer = glGetString( GL_RENDERER );
	const GLubyte* vendor = glGetString( GL_VENDOR );
	const GLubyte* version = glGetString( GL_VERSION );
	const GLubyte* glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv( GL_MAJOR_VERSION, &major );
	glGetIntegerv( GL_MINOR_VERSION, &minor );

	printf( "Renderer Backend: OpenGL\n" );
	printf( "   Vendor            : %s\n", vendor );
	printf( "   Renderer          : %s\n", renderer );
	printf( "   Version (string)  : %s\n", version );
	printf( "   Version (integer) : %d.%d\n", major, minor );
	printf( "   GLSL Version      : %s\n", glslVersion );


	// ATTRIBUTES

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glEnable( GL_PROGRAM_POINT_SIZE );
	
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );
}

void cm::cBackend_OpenGL::clear( unsigned int _color, eClearMode _mode )
{
	float r = (float)( ( _color & 0xFF000000 ) >> 24 ) / 256.0f;
	float g = (float)( ( _color & 0x00FF0000 ) >> 16 ) / 256.0f;
	float b = (float)( ( _color & 0x0000FF00 ) >> 8  ) / 256.0f;
	float a = (float)( ( _color & 0x000000FF ) )       / 256.0f;

	GLenum clearmode = GL_NONE;

	if ( _mode & eClearMode::ClearMode_Color   ) clearmode |= GL_COLOR_BUFFER_BIT;
	if ( _mode & eClearMode::ClearMode_Depth   ) clearmode |= GL_DEPTH_BUFFER_BIT;
	if ( _mode & eClearMode::ClearMode_Stencil ) clearmode |= GL_STENCIL_BUFFER_BIT;
	
	glClearColor( r, g, b, a );
	glClear( clearmode );
}

void cm::cBackend_OpenGL::destroy( void )
{

}

void cm::cBackend_OpenGL::onResize( int _width, int _height )
{
	glViewport( 0, 0, _width, _height );
}

cm::Shader::sShader cm::cBackend_OpenGL::createShader( const char* _source, Shader::eShaderType _type )
{
	int  success;
	char info_log[ 512 ];
	unsigned int shader;

	shader = glCreateShader( getShaderType_OpenGL( _type ) );

	glShaderSource( shader, 1, &_source, NULL );
	glCompileShader( shader );

	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( shader, 512, NULL, info_log );
		printf( "ERROR::SHADER::COMPILATION_FAILED\n %s \n", info_log );
	}

	return { shader, _type };
}

cm::Shader::hShaderProgram cm::cBackend_OpenGL::createShaderProgram()
{
	Shader::hShaderProgram program = glCreateProgram();

	return program;
}

cm::sBuffer cm::cBackend_OpenGL::createBuffer( eBufferType _type, eBufferUsage _usage )
{
	hBuffer buffer = 0;
	glGenBuffers( 1, &buffer );
	glBindBuffer( getBufferTarget_OpenGL( _type ), buffer );

	return { buffer, _type, _usage };
}

cm::hVertexArray cm::cBackend_OpenGL::createVertexArray()
{
	hVertexArray vertex_array;
	glGenVertexArrays( 1, &vertex_array );

	return vertex_array;
}

cm::sTexture2D cm::cBackend_OpenGL::createTexture()
{
	sTexture2D texture;
	glGenTextures( 1, &texture.handle );

	return texture;
}

cm::sFramebuffer cm::cBackend_OpenGL::createFramebuffer( void )
{
	sFramebuffer framebuffer;
	glGenFramebuffers( 1, &framebuffer.handle );

    return framebuffer;
}

void cm::cBackend_OpenGL::attachShader( Shader::hShaderProgram& _program, Shader::sShader& _shader )
{
	glAttachShader( _program, _shader.handle );
}

void cm::cBackend_OpenGL::linkShaderProgram( Shader::hShaderProgram& _program )
{
	int  success;
	char info_log[ 512 ];

	glLinkProgram( _program );

	glGetProgramiv( _program, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( _program, 512, NULL, info_log );
		printf( "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n %s \n", info_log );
	}
}

void cm::cBackend_OpenGL::attachFramebuffer( cm::sFramebuffer& _framebuffer )
{
	glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer.handle );

	int size = (int)_framebuffer.textures.size();

	unsigned int* attachments = new unsigned int[ size ];

	for ( int i = 0; i < size; i++ )
		attachments[ i ] = _framebuffer.textures[ i ].attachment_slot;

	glDrawBuffers( size, attachments );
	delete[] attachments;

	GLenum err = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( err != GL_FRAMEBUFFER_COMPLETE )
	{
		printf( "ERROR::FRAMEBUFFER:: Framebuffer is not complete:\n  " ); // TODO: change to wv::log

		switch ( err )
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         printf( "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n" );         break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: printf( "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n" ); break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        printf( "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n" );        break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        printf( "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n" );        break;
		case GL_FRAMEBUFFER_UNSUPPORTED:                   printf( "GL_FRAMEBUFFER_UNSUPPORTED\n" );                   break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        printf( "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n" );        break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      printf( "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n" );      break;
		}
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void cm::cBackend_OpenGL::generateTexture( sTexture2D _texture, unsigned char* _data )
{
	if ( !_data )
	{
		printf( "No texture data.\n" );
		return;
	}

	glBindTexture( GL_TEXTURE_2D, _texture.handle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	// filtering, TODO: implement as flag
	// glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	GLint format = GL_RGBA;
	switch ( _texture.num_channels )
	{
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _texture.width, _texture.height, 0, format, GL_UNSIGNED_BYTE, _data );
	glGenerateMipmap( GL_TEXTURE_2D );
}

void cm::cBackend_OpenGL::addFramebufferTexture( cm::sFramebuffer& _framebuffer, std::string _name, cm::eTextureFormat _format, cm::eTextureType _type, int _width, int _height )
{
	glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer.handle );

	GLenum format;
	GLenum sized_format;
	GLenum type;
	GLenum attachment_slot = GL_COLOR_ATTACHMENT0 + (int)_framebuffer.textures.size(); // will break if any texture is removed

	if ( _type == FramebufferType_Color )
	{
		
		switch ( _format )
		{
		case FramebufferFormat_R:     sized_format = GL_RED;    format = GL_RED;  type = GL_UNSIGNED_BYTE; break;
		case FramebufferFormat_RG:    sized_format = GL_RG;     format = GL_RG;   type = GL_UNSIGNED_BYTE; break;
		case FramebufferFormat_RGB:   sized_format = GL_RGB;    format = GL_RGB;  type = GL_UNSIGNED_BYTE; break;
		case FramebufferFormat_RGBA:  sized_format = GL_RGBA;   format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;

		case FramebufferFormat_Ri:    sized_format = GL_R32I;    format = GL_RED;  type = GL_UNSIGNED_INT; break;
		case FramebufferFormat_RGi:   sized_format = GL_RG32I;   format = GL_RG;   type = GL_UNSIGNED_INT; break;
		case FramebufferFormat_RGBi:  sized_format = GL_RGB32I;  format = GL_RGB;  type = GL_UNSIGNED_INT; break;
		case FramebufferFormat_RGBAi: sized_format = GL_RGBA32I; format = GL_RGBA; type = GL_UNSIGNED_INT; break;

		case FramebufferFormat_Rf:    sized_format = GL_R16F;    format = GL_RED;  type = GL_FLOAT; break;
		case FramebufferFormat_RGf:   sized_format = GL_RG16F;   format = GL_RG;   type = GL_FLOAT; break;
		case FramebufferFormat_RGBf:  sized_format = GL_RGB16F;  format = GL_RGB;  type = GL_FLOAT; break;
		case FramebufferFormat_RGBAf: sized_format = GL_RGBA16F; format = GL_RGBA; type = GL_FLOAT; break;
		}
	}
	else
	{
		switch ( _type ) // depth and stencil buffer 
		{
		case FramebufferType_Depth:   format = GL_DEPTH_COMPONENT; sized_format = GL_DEPTH_COMPONENT; type = GL_FLOAT; break;
		case FramebufferType_Stencil: format = GL_STENCIL_INDEX;   sized_format = GL_STENCIL_INDEX;   type = GL_BYTE;  break;
		}
	}

	sTexture2D texture;

	glGenTextures( 1, &texture.handle );
	glBindTexture( GL_TEXTURE_2D, texture.handle );
	glTexImage2D( GL_TEXTURE_2D, 0, sized_format, _width, _height, 0, format, type, nullptr );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	switch ( _type )
	{
	case FramebufferType_Color:   glFramebufferTexture2D( GL_FRAMEBUFFER, attachment_slot,             GL_TEXTURE_2D, texture.handle, 0 ); break;
	case FramebufferType_Depth:   glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,         GL_TEXTURE_2D, texture.handle, 0 ); break;
	case FramebufferType_Stencil: glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.handle, 0 ); break;
	}
	
	texture.format = _format;
	texture.attachment_slot = attachment_slot;
	texture.num_channels = _format % 4 + 1;
	texture.type = _type;
	texture.width = _width;
	texture.height = _height;
	texture.name = _name;

	_framebuffer.textures.push_back( texture );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void cm::cBackend_OpenGL::addFramebufferRenderbuffer( cm::sFramebuffer& _framebuffer, cm::eRenderbufferType _type, int _width, int _height )
{

	cm::sRenderbuffer renderbuffer;

	GLenum internalformat = GL_DEPTH_COMPONENT;
	GLenum attachment = GL_DEPTH_ATTACHMENT;
	
	switch ( _type )
	{
	case RenderbufferType_Depth:   internalformat = GL_DEPTH_COMPONENT;    attachment = GL_DEPTH_ATTACHMENT;   break;
	case RenderbufferType_Stencil: internalformat = GL_STENCIL_COMPONENTS; attachment = GL_STENCIL_ATTACHMENT; break;
	// TODO: case DepthStencil: 
	}

	glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer.handle );

	// create renderbuffer object
	glGenRenderbuffers( 1, &renderbuffer.handle );
	glBindRenderbuffer( GL_RENDERBUFFER, renderbuffer.handle );
	glRenderbufferStorage( GL_RENDERBUFFER, internalformat, _width, _height );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	// bind to framebuffer object
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer.handle );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	renderbuffer.width  = _width;
	renderbuffer.height = _height;
	renderbuffer.type = _type;

	_framebuffer.renderbuffers.push_back( renderbuffer );
}

void cm::cBackend_OpenGL::useShaderProgram( Shader::hShaderProgram _program )
{
	glUseProgram( _program );
}

void cm::cBackend_OpenGL::bindFramebuffer( sFramebuffer* _framebuffer )
{
	if ( !_framebuffer )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		return;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer->handle );

	GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 }; // TODO: make better
	glDrawBuffers( (int)_framebuffer->textures.size(), attachments);
	
}

void cm::cBackend_OpenGL::bufferData( sBuffer& _buffer, void* _data, size_t _size )
{
	int target = getBufferTarget_OpenGL( _buffer.type );
	if ( !target )
		return;

	GLenum usage = GL_INVALID_ENUM;
	switch ( _buffer.usage )
	{
	case eBufferUsage::BufferUsage_Static:  usage = GL_STATIC_COPY;  break;
	case eBufferUsage::BufferUsage_Dynamic: usage = GL_DYNAMIC_DRAW; break;
	}

	glBindBuffer( target, _buffer.handle );
	glBufferData( target, _size, _data, usage );

}

void cm::cBackend_OpenGL::bindVertexLayout( cVertexLayout& _layout )
{
	const auto& elements = _layout.getElements();
	unsigned int offset = 0;

	for ( unsigned int i = 0; i < elements.size(); i++ )
	{
		const auto& element = elements[ i ];

		GLenum type = GL_UNSIGNED_BYTE;

		switch ( element.type )
		{
		case cVertexLayout::Float:  type = GL_FLOAT;         break;
		case cVertexLayout::Uint:   type = GL_UNSIGNED_INT;  break;
		case cVertexLayout::Byte:   type = GL_UNSIGNED_BYTE; break;
		case cVertexLayout::Double: type = GL_DOUBLE;        break;
		}

		glEnableVertexAttribArray( i );
		glVertexAttribPointer( i, element.count, type, false, _layout.getStride(), (const void*)offset );
		offset += element.count * (unsigned int)element.size;
	}
}

void cm::cBackend_OpenGL::bindVertexArray( hVertexArray _vertex_array )
{
	glBindVertexArray( _vertex_array );
}

void cm::cBackend_OpenGL::bindTexture2D( hTexture _texture )
{
	glBindTexture( GL_TEXTURE_2D, _texture );
}

void cm::cBackend_OpenGL::bindBuffer( sBuffer _buffer )
{
	glBindBuffer( getBufferTarget_OpenGL( _buffer.type ), _buffer.handle );
}

void cm::cBackend_OpenGL::bindBufferBase( sBuffer _buffer, unsigned int _slot )
{
	glBindBufferBase( getBufferTarget_OpenGL( _buffer.type ), _slot, _buffer.handle );
}

void cm::cBackend_OpenGL::setActiveTextureSlot( int _slot )
{
	glActiveTexture( GL_TEXTURE0 + _slot );
}

void cm::cBackend_OpenGL::drawArrays( unsigned int _vertex_count, eDrawMode _mode )
{
	glDrawArrays( getPrimitive_OpenGL( _mode ), 0, _vertex_count );
}

void cm::cBackend_OpenGL::drawElements( unsigned int _index_count, eDrawMode _mode )
{
	glDrawElements( getPrimitive_OpenGL( _mode ), _index_count, GL_UNSIGNED_INT, 0 );
}

int cm::cBackend_OpenGL::getUniformLocation( Shader::hShaderProgram _program, const char* _uniform )
{
	return glGetUniformLocation( _program, _uniform );;
}

cm::Shader::sShaderUniform cm::cBackend_OpenGL::getUniform( Shader::hShaderProgram _program, unsigned int _slot )
{
	GLint count;

	glGetProgramiv( _program, GL_ACTIVE_UNIFORMS, &count );
	
	if ( _slot >= count )
		return Shader::sShaderUniform{ "NULL", -1, -1, 0 };
	
	const GLsizei buffer_size = 16;

	GLchar name[ buffer_size ];
	GLsizei name_length;
	GLenum type;
	GLint size;
	glGetActiveUniform( _program, (GLuint)_slot, buffer_size, &name_length, &size, &type, name );
	
	return Shader::sShaderUniform{ std::string( name, name_length ), (int)_slot, getShaderType( type ), (size_t)size };
}

cm::Shader::sShaderUniformBlock cm::cBackend_OpenGL::getUniformBlock( Shader::hShaderProgram _program, unsigned int _slot )
{
	GLint num_blocks;
	glGetProgramiv( _program, GL_ACTIVE_UNIFORM_BLOCKS, &num_blocks );

	if ( num_blocks == 0 )
		return { "", 0, 0, {} };

	GLint block_size;
	GLint uniform_count;
	GLint uniforms[64];

	const GLsizei name_buffer_size = 16;
	GLsizei name_length;
	GLchar name[ name_buffer_size ];

	glGetActiveUniformBlockName( _program, _slot, name_buffer_size, &name_length, name );

	glGetActiveUniformBlockiv( _program, _slot, GL_UNIFORM_BLOCK_DATA_SIZE,              &block_size );
	glGetActiveUniformBlockiv( _program, _slot, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,        &uniform_count );
	glGetActiveUniformBlockiv( _program, _slot, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniforms );

	Shader::sShaderUniformBlock uniform_block{ std::string( name ), _slot, block_size, {} };

	for ( int i = 0; i < uniform_count; i++ )
		uniform_block.uniforms.push_back( uniforms[ i ] );
	
	return uniform_block;
}

void cm::cBackend_OpenGL::setUniformBlockBinding( Shader::hShaderProgram _program, const char* _uniform_block, unsigned int _slot )
{
	unsigned int lights_index = glGetUniformBlockIndex( _program, _uniform_block );
	glUniformBlockBinding( _program, lights_index, _slot );
}

void cm::cBackend_OpenGL::setUniformMat4f( int _location, float* _matrix_ptr )
{
	glUniformMatrix4fv( _location, 1, GL_FALSE, _matrix_ptr );
}

void cm::cBackend_OpenGL::setUniformFloat( int _location, float _float )
{
	glUniform1f( _location, _float );
}

void cm::cBackend_OpenGL::setUniformInt( int _location, int _int )
{
	glUniform1i( _location, _int );
}

void cm::cBackend_OpenGL::setUniformVec4f( int _location, wv::cVector4<float> _vector )
{
	glUniform4f( _location, _vector.x, _vector.y, _vector.z, _vector.w );
}

void cm::cBackend_OpenGL::setUniformVec4d( int _location, wv::cVector4<double> _vector )
{
	glUniform4d( _location, _vector.x, _vector.y, _vector.z, _vector.w );
}
