#pragma once

#include <cm/backends/iBackend.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include "cm/Framework/Framebuffer.h"

namespace cm
{
	class cBackend_D3D11 : public iBackend
	{
	public:
		 cBackend_D3D11();
		~cBackend_D3D11();

		void create ( cWindow& _window ) override;
		void clear  ( unsigned int _color, eClearMode _mode ) override;
		void destroy( void ) override;

		void onResize( int _width, int _height ) override;

		void begin( void ) override;
		void end  ( void ) override;

		Shader::sShader        createShader       ( const char* _source, Shader::eShaderType _type ) override;
		Shader::hShaderProgram createShaderProgram( void ) override;
		sBuffer                createBuffer       ( eBufferType _type, eBufferUsage _usage ) override;
		hVertexArray           createVertexArray  ( void ) override;
		sTexture2D             createTexture      ( void ) override;
		sFramebuffer           createFramebuffer  ( void ) override;

		void destroyShaderProgram( Shader::hShaderProgram& _shader ) override;
		void destroyBuffer( sBuffer& _buffer ) override;
		void destroyVertexArray( hVertexArray& _vertex_array ) override;
		void destroyTexture( sTexture2D& _texture ) override;
		void destroyFramebuffer( sFramebuffer& _framebuffer ) override;

		void attachShader( Shader::hShaderProgram& _program, Shader::sShader& _shader ) override;
		void linkShaderProgram( Shader::hShaderProgram& _program ) override;
		
		void attachFramebuffer( cm::sFramebuffer& _buffer ) override;

		void generateTexture( sTexture2D& _texture, unsigned char* _data ) override;

		void addFramebufferTexture     ( cm::sFramebuffer& _buffer, std::string _name, cm::eTextureFormat _format, cm::eTextureType _type, int _width, int _height ) override;
		void addFramebufferRenderbuffer( cm::sFramebuffer& _buffer, cm::eRenderbufferType _type, int _width, int _height ) override;
		
		void bufferData( sBuffer& _buffer, void* _data, size_t _size ) override;

		void useShaderProgram( Shader::hShaderProgram _program ) override;
		void bindFramebuffer( sFramebuffer* _framebuffer ) override;

		void bindVertexLayout( cVertexLayout& _layout ) override;
		void bindVertexArray ( hVertexArray _vertex_array ) override;
		void bindTexture2D   ( hTexture _texture ) override;
		void bindBuffer      ( sBuffer _buffer ) override;
		void bindBufferBase  ( sBuffer _buffer, unsigned int _slot ) override;

		void drawArrays  ( unsigned int _vertex_count, eDrawMode _mode ) override;
		void drawElements( unsigned int _index_count, eDrawMode _mode ) override;

		int                         getUniformLocation( Shader::hShaderProgram _program, const char* _uniform ) override;
		Shader::sUniform      getUniform        ( Shader::hShaderProgram _program, unsigned int _slot ) override;
		Shader::sUniformBlock getUniformBlock   ( Shader::hShaderProgram _program, unsigned int _slot ) override;

		void setActiveTextureSlot( int _slot ) override;

		void setUniformBlockBinding( Shader::hShaderProgram _program, const char* _uniform, unsigned int _slot ) override;
		void setUniformMat4f( int _location, float* _matrix_ptr ) override;
		void setUniformFloat( int _location, float _float ) override;
		void setUniformInt  ( int _location, int _int ) override;
		void setUniformVec4f( int _location, wv::cVector4<float> _vector ) override;
		void setUniformVec4d( int _location, wv::cVector4<double> _vector ) override;

	private:

		int createSwapchainResources  ( void );
		void destroySwapchainResources( void );

		cWindow* m_window;

		Microsoft::WRL::ComPtr<IDXGIFactory2>          m_dxgi_factory   = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Device>           m_device         = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_device_context = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>        m_swap_chain     = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target  = nullptr;

	};
}
