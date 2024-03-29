#pragma once

#include <cm/backends/iBackend.h>
#include <map>

namespace cm
{
	class cBackend_OpenGL : public iBackend
	{
	public:
		 cBackend_OpenGL();
		~cBackend_OpenGL();

		void create( cWindow& _window ) override;
		void clear( unsigned int _color, eClearMode _mode ) override;
		void destroy( void ) override;
	
		void onResize( int _width, int _height ) override;

		void begin( void ) override;
		void end  ( void ) override;
		void printdebug( void ) override;

		Shader::sShader        createShader       ( std::string& _source, Shader::eShaderType _type ) override;
		Shader::hShaderProgram createShaderProgram( void ) override;
		sBuffer                createBuffer       ( eBufferType _type, eBufferUsage _usage ) override;
		hVertexArray           createVertexArray  ( void ) override;
		sTexture2D             createTexture      ( void ) override;
		sTexture2D             createTexture      ( cm::eTextureFormat _format, cm::eTextureTarget _target, cm::eTextureType _type, int _width, int _height ) override;
		sFramebuffer           createFramebuffer  ( void ) override;

		void destroyShader( Shader::sShader _shader ) override;
		void destroyShaderProgram( Shader::hShaderProgram& _shader ) override;
		void destroyBuffer( sBuffer& _buffer ) override;
		void destroyVertexArray( hVertexArray& _vertex_array ) override;
		void destroyTexture( sTexture2D& _texture ) override;
		void destroyFramebuffer( sFramebuffer& _framebuffer ) override;

		void attachShader( Shader::hShaderProgram& _program, Shader::sShader& _shader ) override;
		void linkShaderProgram( Shader::hShaderProgram& _program ) override;
		
		void attachFramebuffer( cm::sFramebuffer& _framebuffer ) override;

		void generateTexture( sTexture2D& _texture, unsigned char* _data ) override;

		void addFramebufferTexture     ( cm::sFramebuffer& _framebuffer, std::string _name, cm::sTexture2D _texture ) override;
		void addFramebufferRenderbuffer( cm::sFramebuffer& _framebuffer, cm::eRenderbufferType _type, int _width, int _height ) override;
		
		void bufferData( sBuffer& _buffer, void* _data, size_t _size ) override;

		void useShaderProgram( Shader::hShaderProgram _program ) override;

		void bindFramebuffer ( sFramebuffer& _framebuffer ) override;
		void bindVertexLayout( cVertexLayout& _layout ) override;
		void bindVertexArray ( hVertexArray _vertex_array ) override;
		void bindTexture2D   ( sTexture2D& _texture ) override;
		void bindBuffer      ( sBuffer& _buffer ) override;
		void bindBufferBase  ( sBuffer& _buffer, unsigned int _slot ) override;

		void unbindFramebuffer( void ) override;
		void unbindVertexArray( void ) override;
		void unbindTexture2D  ( sTexture2D& _texture ) override;
		void unbindBuffer     ( sBuffer& _buffer ) override;
		
		void drawArrays  ( unsigned int _vertex_count, eDrawMode _mode ) override;
		void drawElements( unsigned int _index_count, eDrawMode _mode ) override;
		void drawElementsInstanced( unsigned int _index_count, eDrawMode _mode, int _count ) override;
		void blitFramebuffer( sFramebuffer& _framebuffer_read, sFramebuffer& _framebuffer_write ) override;

		int                   getUniformLocation( Shader::hShaderProgram _program, const char* _uniform ) override;
		Shader::sUniform      getUniform        ( Shader::hShaderProgram _program, unsigned int _slot ) override;
		Shader::sUniformBlock getUniformBlock   ( Shader::hShaderProgram _program, unsigned int _slot ) override;

		void setActiveTextureSlot( int _slot ) override;

		void setUniformBlockBinding( Shader::hShaderProgram _program, const char* _uniform, unsigned int _slot ) override;
		void setUniformMat4f( int _location, float* _matrix_ptr ) override;
		void setUniformFloat( int _location, float _float ) override;
		void setUniformInt  ( int _location, int _int ) override;
		void setUniformVec4f( int _location, wv::cVector4<float> _vector ) override;
		void setUniformVec4d( int _location, wv::cVector4<double> _vector ) override;

		int getUniformInt( Shader::hShaderProgram _program, int _location ) override;

	private:

		int m_framebuffer_textures = 0;
		int m_max_framebuffer_textures = 32; // TODO: query

		int m_num_shaders         = 0;
		int m_num_shader_programs = 0;
		int m_num_buffers         = 0;
		int m_num_vertex_arrays   = 0;
		int m_num_textures        = 0;
		int m_num_framebuffers    = 0;
		int m_num_renderbuffers   = 0;

		Shader::hShaderProgram m_currently_bound_program = 0;

	};
}
