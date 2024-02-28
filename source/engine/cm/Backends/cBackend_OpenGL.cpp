#include "cBackend_OpenGL.h"

#include <glad/glad.h>

#include <cm/Core/cWindow.h>
#include <cm/Framework/cVertexLayout.h>

#include <stdio.h>
#include <string>

static GLuint getShaderType_OpenGL( cm::Shader::eShaderType _type )
{
	switch ( _type )
	{
	case cm::Shader::ShaderType_Vertex:   return GL_VERTEX_SHADER;   break;
	case cm::Shader::ShaderType_Fragment: return GL_FRAGMENT_SHADER; break;
	case cm::Shader::ShaderType_Geometry: return GL_GEOMETRY_SHADER; break;
	}

	return GL_NONE;
}

static GLuint getBufferTarget_OpenGL( cm::eBufferType _type )
{
	switch ( _type )
	{
	case cm::BufferType_Vertex:  return GL_ARRAY_BUFFER;         break;
	case cm::BufferType_Index:   return GL_ELEMENT_ARRAY_BUFFER; break;
	case cm::BufferType_Uniform: return GL_UNIFORM_BUFFER;       break;
	}

	return 0;
}

static GLuint getPrimitive_OpenGL( cm::eDrawMode _mode )
{
	switch ( _mode )
	{
	case cm::DrawMode_Lines:     return GL_LINES;      break;
	case cm::DrawMode_LineLoop:  return GL_LINE_LOOP;  break;
	case cm::DrawMode_LineStrip: return GL_LINE_STRIP; break;
	case cm::DrawMode_Points:    return GL_POINTS;     break;
	case cm::DrawMode_Triangle:  return GL_TRIANGLES;  break;
	}

	return GL_LINES;
}

static void getTextureFormat_OpenGL( cm::eTextureFormat _format, GLenum* _format_out, GLenum* _sized_format_out )
{
	switch ( _format )
	{
	case cm::TextureFormat_R:     *_sized_format_out = GL_RED;    *_format_out = GL_RED;  break;
	case cm::TextureFormat_RG:    *_sized_format_out = GL_RG;     *_format_out = GL_RG;   break;
	case cm::TextureFormat_RGB:   *_sized_format_out = GL_RGB;    *_format_out = GL_RGB;  break;
	case cm::TextureFormat_RGBA:  *_sized_format_out = GL_RGBA;   *_format_out = GL_RGBA; break;

	case cm::TextureFormat_Ri:    *_sized_format_out = GL_R32I;    *_format_out = GL_RED;  break;
	case cm::TextureFormat_RGi:   *_sized_format_out = GL_RG32I;   *_format_out = GL_RG;   break;
	case cm::TextureFormat_RGBi:  *_sized_format_out = GL_RGB32I;  *_format_out = GL_RGB;  break;
	case cm::TextureFormat_RGBAi: *_sized_format_out = GL_RGBA32I; *_format_out = GL_RGBA; break;

	case cm::TextureFormat_Rf:    *_sized_format_out = GL_R16F;    *_format_out = GL_RED;  break;
	case cm::TextureFormat_RGf:   *_sized_format_out = GL_RG16F;   *_format_out = GL_RG;   break;
	case cm::TextureFormat_RGBf:  *_sized_format_out = GL_RGB16F;  *_format_out = GL_RGB;  break;
	case cm::TextureFormat_RGBAf: *_sized_format_out = GL_RGBA16F; *_format_out = GL_RGBA; break;
	}
}

static GLenum getTextureFormatType_OpenGL( cm::eTextureFormat _format )
{
	GLenum type;

	switch ( _format )
	{
	case cm::TextureFormat_R:  case cm::TextureFormat_RG:  case cm::TextureFormat_RGB:  case cm::TextureFormat_RGBA:  type = GL_UNSIGNED_BYTE; break;
	case cm::TextureFormat_Ri: case cm::TextureFormat_RGi: case cm::TextureFormat_RGBi: case cm::TextureFormat_RGBAi: type = GL_UNSIGNED_INT;  break;
	case cm::TextureFormat_Rf: case cm::TextureFormat_RGf: case cm::TextureFormat_RGBf: case cm::TextureFormat_RGBAf: type = GL_FLOAT;         break;
	}

	return type;
}

static GLenum getTextureTarget_OpenGL( cm::eTextureTarget _target )
{
	switch ( _target )
	{
	case cm::TextureTarget_Texture2D:            return GL_TEXTURE_2D; break;
	case cm::TextureTarget_Texture2DMultisample: return GL_TEXTURE_2D_MULTISAMPLE; break;
	}

	return GL_INVALID_ENUM;
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


//////////// tjomgojaeoirjogntaoiejr


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

	const GLubyte* renderer    = glGetString( GL_RENDERER );
	const GLubyte* vendor      = glGetString( GL_VENDOR );
	const GLubyte* version     = glGetString( GL_VERSION );
	const GLubyte* glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	GLint max_uniform_block_bindings; 
	GLint max_uniform_blocks_v, max_uniform_blocks_f, max_uniform_blocks_g;
	GLint max_uniform_block_size;

	glGetIntegerv( GL_MAJOR_VERSION, &major );
	glGetIntegerv( GL_MINOR_VERSION, &minor );

	glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_block_bindings );
	glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE,      &max_uniform_block_size );
	glGetIntegerv( GL_MAX_VERTEX_UNIFORM_BLOCKS,   &max_uniform_blocks_v );
	glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &max_uniform_blocks_f );
	glGetIntegerv( GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &max_uniform_blocks_g );

	printf( "OpenGL Information\n" );
	printf( "    Vendor            : %s\n",    vendor );
	printf( "    Renderer          : %s\n",    renderer );
	printf( "    Version (string)  : %s\n",    version );
	printf( "    Version (integer) : %d.%d\n", major, minor );
	printf( "    GLSL Version      : %s\n",    glslVersion );
	printf( "\n" );
	printf( "Hardware Information\n" );
	printf( "    Max Uniform Block Bindings : %i\n", max_uniform_block_bindings );
	printf( "    Max Uniform Block Size     : %i\n", max_uniform_block_size );
	printf( "    Max Vert Uniform Blocks    : %i\n", max_uniform_blocks_v );
	printf( "    Max Frag Uniform Blocks    : %i\n", max_uniform_blocks_f );
	printf( "    Max Geom Uniform Blocks    : %i\n\n", max_uniform_blocks_g );
	


	// ATTRIBUTES

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	glEnable( GL_MULTISAMPLE );

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

void cm::cBackend_OpenGL::begin( void )
{
}

void cm::cBackend_OpenGL::end( void )
{
	//glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	//glBindFramebuffer( GL_READ_FRAMEBUFFER, fbo );
	glDrawBuffer( GL_BACK );
	glBlitFramebuffer( 0, 0, 1000, 1000, 0, 0, 1000, 1000, GL_COLOR_BUFFER_BIT, GL_LINEAR );

	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	glBindVertexArray( 0 );
	glUseProgram( 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void cm::cBackend_OpenGL::printdebug( void )
{
	printf( "OpenGL Debug:\n\n" );
	printf( "  num shaders         : %i\n",   m_num_shaders         );
	printf( "  num shader programs : %i\n",   m_num_shader_programs );
	printf( "  num buffers         : %i\n",   m_num_buffers         );
	printf( "  num vertex arrays   : %i\n",   m_num_vertex_arrays   );
	printf( "  num textures        : %i\n",   m_num_textures        );
	printf( "  num framebuffers    : %i\n",   m_num_framebuffers    );
	printf( "  num renderbuffers   : %i\n\n", m_num_renderbuffers   );
}

cm::Shader::sShader cm::cBackend_OpenGL::createShader( std::string& _source, Shader::eShaderType _type )
{
	int  success;
	char info_log[ 512 ];
	cm::Shader::hShader shader;

	GLenum shader_type = getShaderType_OpenGL( _type );
	// vert 35633
	// frag 35632
	// geom 36313

	shader = glCreateShader( shader_type );

	const char* source = _source.data();
	glShaderSource( shader, 1, &source, NULL );
	glCompileShader( shader );

	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( shader, 512, NULL, info_log );
		printf( "Error:\n%s\n", info_log);
	}

	m_num_shaders++;
	return { shader, _type };
}

cm::Shader::hShaderProgram cm::cBackend_OpenGL::createShaderProgram()
{
	Shader::hShaderProgram program = glCreateProgram();
	
	m_num_shader_programs++;
	return program;
}

cm::sBuffer cm::cBackend_OpenGL::createBuffer( eBufferType _type, eBufferUsage _usage )
{
	hBuffer buffer = 0;
	glGenBuffers( 1, &buffer );
	glBindBuffer( getBufferTarget_OpenGL( _type ), buffer );

	m_num_buffers++;
	return { buffer, _type, _usage };
}

cm::hVertexArray cm::cBackend_OpenGL::createVertexArray()
{
	hVertexArray vertex_array;
	glGenVertexArrays( 1, &vertex_array );

	m_num_vertex_arrays++;
	return vertex_array;
}

cm::sTexture2D cm::cBackend_OpenGL::createTexture()
{
	sTexture2D texture;
	glGenTextures( 1, &texture.handle );

	m_num_textures++;
	return texture;
}

cm::sTexture2D cm::cBackend_OpenGL::createTexture( cm::eTextureFormat _format, cm::eTextureTarget _target, cm::eTextureType _type, int _width, int _height )
{

	GLenum format;
	GLenum sized_format;
	GLenum type;
	GLenum target;

	m_framebuffer_textures++;

	if ( _type == TextureType_Color )
	{
		getTextureFormat_OpenGL( _format, &format, &sized_format );
		type = getTextureFormatType_OpenGL( _format );
	}
	else
	{
		switch ( _type ) // depth and stencil buffer 
		{
		case TextureType_Depth:   format = GL_DEPTH_COMPONENT; sized_format = GL_DEPTH_COMPONENT32; type = GL_FLOAT; break;
		case TextureType_Stencil: format = GL_STENCIL_INDEX;   sized_format = GL_STENCIL_INDEX;     type = GL_BYTE;  break;
		}
	}

	target = getTextureTarget_OpenGL( _target );

	sTexture2D texture;

	glGenTextures( 1, &texture.handle );
	glBindTexture( target, texture.handle );

	switch ( _target )
	{
	case TextureTarget_Texture2D:            glTexImage2D( target, 0, sized_format, _width, _height, 0, format, type, nullptr );  break;
	case TextureTarget_Texture2DMultisample: glTexImage2DMultisample( target, 3, sized_format, _width, _height, false ); break;
	}
	
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	texture.format = _format;
	texture.num_channels = _format % 4 + 1;
	texture.type = _type;
	texture.width = _width;
	texture.height = _height;
	texture.target = _target;

	return texture;
}

cm::sFramebuffer cm::cBackend_OpenGL::createFramebuffer( void )
{
	sFramebuffer framebuffer;
	glGenFramebuffers( 1, &framebuffer.handle );

	m_num_framebuffers++;
    return framebuffer;
}

void cm::cBackend_OpenGL::destroyShader( Shader::sShader _shader )
{
	glDeleteShader( _shader.handle );
	m_num_shaders--;
}

void cm::cBackend_OpenGL::destroyShaderProgram( Shader::hShaderProgram& _shader )
{
	glDeleteProgram( _shader );
	m_num_shader_programs--;
}

void cm::cBackend_OpenGL::destroyBuffer( sBuffer& _buffer )
{
	glDeleteBuffers( 1, &_buffer.handle );
	m_num_buffers--;
}

void cm::cBackend_OpenGL::destroyVertexArray( hVertexArray& _vertex_array )
{
	glDeleteVertexArrays( 1, &_vertex_array );
	m_num_vertex_arrays--;
}

void cm::cBackend_OpenGL::destroyTexture( sTexture2D& _texture )
{
	glDeleteTextures( 1, &_texture.handle );
	m_num_textures--;
}

void cm::cBackend_OpenGL::destroyFramebuffer( sFramebuffer& _framebuffer )
{
	for ( int i = 0; i < _framebuffer.textures.size(); i++ )
		destroyTexture( _framebuffer.textures[ i ] );
	
	for ( int i = 0; i < _framebuffer.renderbuffers.size(); i++ )
		glDeleteRenderbuffers( 1, &_framebuffer.renderbuffers[ i ].handle );
	
	_framebuffer.textures.clear();

	glDeleteFramebuffers( 1, &_framebuffer.handle );
	m_num_framebuffers--;
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

void cm::cBackend_OpenGL::generateTexture( sTexture2D& _texture, unsigned char* _data )
{
	if ( !_data )
	{
		glDeleteTextures( 1, &_texture.handle );
		_texture.handle = 0;
		return;
	}

	glBindTexture( GL_TEXTURE_2D, _texture.handle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	// filtering, TODO: implement as flag
	// glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
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

	_texture.target = TextureTarget_Texture2D;
}

void cm::cBackend_OpenGL::addFramebufferTexture( cm::sFramebuffer& _framebuffer, std::string _name, cm::sTexture2D _texture )
{
	glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer.handle );

	GLenum attachment_slot = GL_COLOR_ATTACHMENT0 + _framebuffer.textures.size(); // m_framebuffer_textures;
	m_framebuffer_textures++;

	if( _texture.type == TextureType_Depth )   attachment_slot = GL_DEPTH_ATTACHMENT;
	if( _texture.type == TextureType_Stencil ) attachment_slot = GL_DEPTH_STENCIL_ATTACHMENT;
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, attachment_slot, getTextureTarget_OpenGL( _texture.target ), _texture.handle, 0 );

	_texture.attachment_slot = attachment_slot;
	_texture.name = _name;

	_framebuffer.textures.push_back( _texture );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void cm::cBackend_OpenGL::addFramebufferRenderbuffer( cm::sFramebuffer& _framebuffer, cm::eRenderbufferType _type, int _width, int _height )
{

	cm::sRenderbuffer renderbuffer;

	GLenum internalformat = GL_DEPTH_COMPONENT;
	GLenum attachment     = GL_DEPTH_ATTACHMENT;
	
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

	if ( _framebuffer.samples != 0 ) glRenderbufferStorageMultisample( GL_RENDERBUFFER, _framebuffer.samples, internalformat, _width, _height );
	else                             glRenderbufferStorage( GL_RENDERBUFFER, internalformat, _width, _height );

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
	if ( _program != m_currently_bound_program )
	{
		glUseProgram( _program );
		m_currently_bound_program = _program;
	}
}

void cm::cBackend_OpenGL::bindFramebuffer( sFramebuffer& _framebuffer )
{
	glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer.handle );

	std::vector<GLuint> attachments;

	for ( int i = 0; i < (int)_framebuffer.textures.size(); i++ )
		attachments.push_back( GL_COLOR_ATTACHMENT0 + i );
	
	glDrawBuffers( (int)attachments.size(), attachments.data() );
}

void cm::cBackend_OpenGL::bufferData( sBuffer& _buffer, void* _data, size_t _size )
{
	int target = getBufferTarget_OpenGL( _buffer.type );
	
	if ( target == 0 || _buffer.handle == 0 )
		return;

	GLenum usage = 0;
	switch ( _buffer.usage )
	{
	case eBufferUsage::BufferUsage_Static:  usage = GL_STATIC_DRAW;  break;
	case eBufferUsage::BufferUsage_Dynamic: usage = GL_DYNAMIC_DRAW; break;
	}
	if ( usage == 0 )
		return;

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

void cm::cBackend_OpenGL::bindTexture2D( sTexture2D& _texture )
{
	glBindTexture( getTextureTarget_OpenGL( _texture.target ), _texture.handle );
}

void cm::cBackend_OpenGL::bindBuffer( sBuffer& _buffer )
{
	glBindBuffer( getBufferTarget_OpenGL( _buffer.type ), _buffer.handle );
}

void cm::cBackend_OpenGL::bindBufferBase( sBuffer& _buffer, unsigned int _slot )
{
	glBindBufferBase( getBufferTarget_OpenGL( _buffer.type ), _slot, _buffer.handle );
}

void cm::cBackend_OpenGL::unbindFramebuffer( void )
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void cm::cBackend_OpenGL::unbindVertexArray( void )
{
	glBindVertexArray( 0 );
}

void cm::cBackend_OpenGL::unbindTexture2D( sTexture2D& _texture )
{
	glBindTexture( getTextureTarget_OpenGL( _texture.target ), 0 );
}

void cm::cBackend_OpenGL::unbindBuffer( sBuffer& _buffer )
{

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

void cm::cBackend_OpenGL::drawElementsInstanced( unsigned int _index_count, eDrawMode _mode, int _count )
{
	glDrawElementsInstanced( getPrimitive_OpenGL( _mode ), _index_count, GL_UNSIGNED_INT, 0, _count );
}

void cm::cBackend_OpenGL::blitFramebuffer( sFramebuffer& _framebuffer_read, sFramebuffer& _framebuffer_write )
{
	glBindFramebuffer( GL_READ_FRAMEBUFFER, _framebuffer_read.handle );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _framebuffer_write.handle );

	for ( int i = 0; i < _framebuffer_read.textures.size(); i++ )
	{
		glReadBuffer( GL_COLOR_ATTACHMENT0 + i );
		glDrawBuffer( GL_COLOR_ATTACHMENT0 + i );

		int src_width = _framebuffer_read.textures[ i ].width;
		int src_height = _framebuffer_read.textures[ i ].height;

		int dst_width = _framebuffer_write.textures[ i ].width;
		int dst_height = _framebuffer_write.textures[ i ].height;

		glBlitFramebuffer( 0, 0, src_width, src_height, 0, 0, dst_width, dst_height, GL_COLOR_BUFFER_BIT, GL_LINEAR );
	}
	
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}

int cm::cBackend_OpenGL::getUniformLocation( Shader::hShaderProgram _program, const char* _uniform )
{
	return glGetUniformLocation( _program, _uniform );;
}

cm::Shader::sUniform cm::cBackend_OpenGL::getUniform( Shader::hShaderProgram _program, unsigned int _slot )
{
	GLint count;

	glGetProgramiv( _program, GL_ACTIVE_UNIFORMS, &count );
	
	if ( _slot >= count )
		return Shader::sUniform{ "NULL", -1, -1, 0 };
	
	const GLsizei buffer_size = 72;

	GLchar name[ buffer_size ];
	GLsizei name_length;
	GLenum type;
	GLint size;
	glGetActiveUniform( _program, (GLuint)_slot, buffer_size, &name_length, &size, &type, name );
	
	return Shader::sUniform{ std::string( name, name_length ), (int)_slot, getShaderType( type ), (size_t)size };
}

cm::Shader::sUniformBlock cm::cBackend_OpenGL::getUniformBlock( Shader::hShaderProgram _program, unsigned int _slot )
{
	GLint num_blocks;
	glGetProgramiv( _program, GL_ACTIVE_UNIFORM_BLOCKS, &num_blocks );

	if ( num_blocks == 0 )
		return { "", 0, 0, {} };

	GLint block_size;
	GLint uniform_count;
	GLint uniforms[512];

	const GLsizei name_buffer_size = 128;
	GLsizei name_length;
	GLchar name[ name_buffer_size ];

	glGetActiveUniformBlockName( _program, _slot, name_buffer_size, &name_length, name );

	glGetActiveUniformBlockiv( _program, _slot, GL_UNIFORM_BLOCK_DATA_SIZE,              &block_size );
	glGetActiveUniformBlockiv( _program, _slot, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,        &uniform_count );
	glGetActiveUniformBlockiv( _program, _slot, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniforms );

	Shader::sUniformBlock uniform_block{ std::string( name ), _slot, block_size, {} };

	for ( int i = 0; i < uniform_count; i++ )
	{
		Shader::sUniformBlockVariable variable;
		variable.name = getUniform( _program, uniforms[ i ] ).name;
		variable.location = uniforms[ i ];

		GLenum props[] = { GL_OFFSET };
		glGetProgramResourceiv( _program, GL_UNIFORM, variable.location, 1, props, 1, nullptr, &variable.offset );

		uniform_block.uniforms[ variable.name ] = variable;
	}
	
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

int cm::cBackend_OpenGL::getUniformInt( Shader::hShaderProgram _program, int _location )
{
	int val = -1;
	glGetUniformiv( _program, _location, &val );
    return val;
}
