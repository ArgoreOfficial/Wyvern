#include "cBackend_D3D11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

#include <stdio.h>

#include <cm/Core/cWindow.h>

cm::cBackend_D3D11::cBackend_D3D11() :
	m_window{ nullptr }
{

}

cm::cBackend_D3D11::~cBackend_D3D11()
{

}

void cm::cBackend_D3D11::create( cWindow& _window )
{
	m_window = &_window;

	if ( FAILED( CreateDXGIFactory1( IID_PPV_ARGS( &m_dxgi_factory ) ) ) )
	{
		printf( "DXGI: Unable to create DXGIFactory\n" );
		return;
	}

	constexpr D3D_FEATURE_LEVEL deviceFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	/* I hate windows code */
	if ( FAILED( D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &deviceFeatureLevel, 1, D3D11_SDK_VERSION, &m_device, nullptr, &m_device_context ) ) )
	{
		printf( "D3D11: Failed to create device and device Context\n" );
		return;
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
	swapChainDescriptor.Width = _window.getWidth();
	swapChainDescriptor.Height = _window.getHeight();
	swapChainDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDescriptor.SampleDesc.Count = 1;
	swapChainDescriptor.SampleDesc.Quality = 0;
	swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescriptor.BufferCount = 2;
	swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDescriptor.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
	swapChainDescriptor.Flags = {};

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDescriptor = {};
	swapChainFullscreenDescriptor.Windowed = true;

	if ( FAILED( m_dxgi_factory->CreateSwapChainForHwnd( m_device.Get(), glfwGetWin32Window( _window.getWindowObject() ), &swapChainDescriptor, &swapChainFullscreenDescriptor, nullptr, &m_swap_chain ) ) )
	{
		printf( "DXGI: Failed to create swapchain\n" );
		return;
	}

	if ( !createSwapchainResources() )
	{
		return;
	}

	printf( "Created D3D11 Renderer.\n" );
}

void cm::cBackend_D3D11::clear( unsigned int _color )
{
	const float r = ( _color & 0xFF000000 ) / 256.0f;
	const float g = ( _color & 0x00FF0000 ) / 256.0f;
	const float b = ( _color & 0x0000FF00 ) / 256.0f;
	const float a = ( _color & 0x000000FF ) / 256.0f;

	const float clear_color[ 4 ] = { r, g, b, a };

	m_device_context->ClearRenderTargetView( m_render_target.Get(), clear_color );
}

void cm::cBackend_D3D11::begin( void )
{
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>( m_window->getWidth() );
	viewport.Height = static_cast<float>( m_window->getHeight() );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_device_context->RSSetViewports( 1, &viewport );

	m_device_context->OMSetRenderTargets( 1, m_render_target.GetAddressOf(), nullptr );
}

void cm::cBackend_D3D11::end( void )
{
	m_swap_chain->Present( 1, 0 );
}

cm::Shader::sShader cm::cBackend_D3D11::createShader( const char* _source, Shader::eShaderType _type )
{
	return Shader::sShader{};
}

cm::Shader::hShaderProgram cm::cBackend_D3D11::createShaderProgram()
{
	return Shader::hShaderProgram{};
}

cm::sBuffer cm::cBackend_D3D11::createBuffer( eBufferType _type )
{
	return sBuffer{};
}

cm::hVertexArray cm::cBackend_D3D11::createVertexArray()
{
	return hVertexArray{};
}

cm::sTexture2D cm::cBackend_D3D11::createTexture()
{
	return sTexture2D{};
}

void cm::cBackend_D3D11::attachShader( Shader::hShaderProgram& _program, Shader::sShader& _shader )
{
}

void cm::cBackend_D3D11::linkShaderProgram( Shader::hShaderProgram& _program )
{
}

void cm::cBackend_D3D11::generateTexture( sTexture2D _texture, unsigned char* _data )
{
}

void cm::cBackend_D3D11::bufferData( sBuffer& _buffer, void* _data, size_t _size )
{
}

void cm::cBackend_D3D11::useShaderProgram( Shader::hShaderProgram _program )
{
}

void cm::cBackend_D3D11::bindVertexLayout( cVertexLayout& _layout )
{
}

void cm::cBackend_D3D11::bindVertexArray( hVertexArray _vertex_array )
{
}

void cm::cBackend_D3D11::bindTexture2D( hTexture _texture )
{
}

void cm::cBackend_D3D11::setActiveTextureSlot( int _slot )
{
}

void cm::cBackend_D3D11::drawArrays( unsigned int _vertex_count, eDrawMode _mode )
{
}

void cm::cBackend_D3D11::drawElements( unsigned int _index_count, eDrawMode _mode )
{
}

int cm::cBackend_D3D11::getUniformLocation( Shader::hShaderProgram _shader, const char* _uniform )
{
	return -1;
}

cm::Shader::sShaderUniform cm::cBackend_D3D11::getUniform( Shader::hShaderProgram _program, unsigned int _slot )
{
	return Shader::sShaderUniform{ "", -1, -1 };
}

void cm::cBackend_D3D11::setUniformMat4f( int _location, float* _matrix_ptr )
{
}

void cm::cBackend_D3D11::setUniformFloat( int _location, float _float )
{
}

void cm::cBackend_D3D11::setUniformInt( int _location, int _int )
{
}

void cm::cBackend_D3D11::setUniformVec4f( int _location, wv::cVector4<float> _vector )
{
}

void cm::cBackend_D3D11::setUniformVec4d( int _location, wv::cVector4<double> _vector )
{
}

int cm::cBackend_D3D11::createSwapchainResources()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	if ( FAILED( m_swap_chain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) ) ) )
	{
		printf( "D3D11: Failed to get Back Buffer from the SwapChain\n" );
		return 0;
	}

	if ( FAILED( m_device->CreateRenderTargetView( backBuffer.Get(), nullptr, &m_render_target ) ) )
	{
		printf( "D3D11: Failed to create RTV from Back Buffer\n" );
		return 0;
	}

	return 1;
}

void cm::cBackend_D3D11::destroySwapchainResources()
{
	m_render_target.Reset();
}
