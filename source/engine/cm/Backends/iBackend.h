#pragma once

#include <cm/Framework/Buffer.h>
#include <cm/Framework/Framebuffer.h>
#include <cm/Framework/Shader.h>
#include <cm/Framework/Texture.h>
#include <cm/Framework/VertexArray.h>
#include <cm/Framework/Renderbuffer.h>

#include <wv/Math/Vector4.h>

namespace cm
{
	class cWindow;
	class cVertexLayout;

	enum eDrawMode
	{
		DrawMode_Points,
		DrawMode_Lines,
		DrawMode_LineStrip,
		DrawMode_LineLoop,
		DrawMode_Triangle
	};

	enum eClearMode
	{
		ClearMode_Color   = 0b001,
		ClearMode_Depth   = 0b010,
		ClearMode_Stencil = 0b100
	};

	class iBackend
	{
	public:
		virtual ~iBackend() { }

		virtual void create ( cWindow& _window ) = 0;
		virtual void clear  ( unsigned int _color, eClearMode _mode ) = 0;
		virtual void destroy( void ) = 0;

		virtual void onResize( int _width, int _height ) { }

		virtual void begin( void ) { }
		virtual void end  ( void ) { }

		virtual Shader::sShader        createShader       ( const char* _source, Shader::eShaderType _type ) = 0;
		virtual Shader::hShaderProgram createShaderProgram( void ) = 0;
		virtual sBuffer                createBuffer       ( eBufferType _type, eBufferUsage _usage ) = 0;
		virtual hVertexArray           createVertexArray  ( void ) = 0;
		virtual sTexture2D             createTexture      ( void ) = 0;
		virtual sFramebuffer           createFramebuffer  ( void ) = 0;

		virtual void destroyShaderProgram( Shader::hShaderProgram& _shader ) = 0;
		virtual void destroyBuffer( sBuffer& _buffer ) = 0;
		virtual void destroyVertexArray( hVertexArray& _vertex_array ) = 0;
		virtual void destroyTexture( sTexture2D& _texture ) = 0;
		virtual void destroyFramebuffer( sFramebuffer& _framebuffer ) = 0;

		virtual void attachShader     ( Shader::hShaderProgram& _program, Shader::sShader& _shader ) = 0;
		virtual void linkShaderProgram( Shader::hShaderProgram& _program ) = 0;
		
		virtual void attachFramebuffer( cm::sFramebuffer& _framebuffer ) = 0;
		
		virtual void generateTexture( sTexture2D _texture, unsigned char* _data ) = 0;

		virtual void addFramebufferTexture     ( cm::sFramebuffer& _buffer, std::string _name, cm::eTextureFormat _format, cm::eTextureType _type, int _width, int _height ) = 0;
		virtual void addFramebufferRenderbuffer( cm::sFramebuffer& _buffer, cm::eRenderbufferType _type, int _width, int _height ) = 0;
		
		virtual void bufferData( sBuffer& _buffer, void* _data, size_t _size ) = 0;

		virtual void useShaderProgram    ( Shader::hShaderProgram _program ) = 0;
		virtual void bindFramebuffer      ( sFramebuffer* _framebuffer ) = 0;
		
		virtual void bindVertexLayout    ( cVertexLayout& _layout ) = 0;
		virtual void bindVertexArray     ( hVertexArray _vertex_array ) = 0;
		virtual void bindTexture2D       ( hTexture _texture ) = 0;
		virtual void bindBuffer          ( sBuffer _buffer ) = 0;
		virtual void bindBufferBase      ( sBuffer _buffer, unsigned int _slot ) = 0;

		virtual void drawArrays  ( unsigned int _vertex_count, eDrawMode _mode ) = 0;
		virtual void drawElements( unsigned int _index_count, eDrawMode _mode ) = 0;

		virtual int                         getUniformLocation( Shader::hShaderProgram _program, const char* _uniform ) = 0;
		virtual Shader::sUniform      getUniform        ( Shader::hShaderProgram _program, unsigned int _slot ) = 0;
		virtual Shader::sUniformBlock getUniformBlock   ( Shader::hShaderProgram _program, unsigned int _slot ) = 0;

		virtual void setActiveTextureSlot( int _slot ) = 0;

		virtual void setUniformBlockBinding( Shader::hShaderProgram _program, const char* _uniform, unsigned int _slot ) = 0;
		virtual void setUniformMat4f       ( int _location, float* _matrix_ptr ) = 0;
		virtual void setUniformFloat       ( int _location, float _float ) = 0;
		virtual void setUniformInt         ( int _location, int _int ) = 0;
		virtual void setUniformVec4f       ( int _location, wv::cVector4<float> _vector ) = 0;
		virtual void setUniformVec4d       ( int _location, wv::cVector4<double> _vector ) = 0;

	protected:
		iBackend() { }

	private:

	};
}
