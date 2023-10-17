#include "Window.h"

#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <Wyvern/Managers/EventManager.h>

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

using namespace WV;

Window::Window() :
	m_window( nullptr )
{

}

Window::~Window()
{

}


void Window::windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	bgfx::reset( (uint32_t)_width, (uint32_t)_height, m_vsync_enabled ? BGFX_RESET_VSYNC : BGFX_RESET_NONE );
	// glfwGetWindowSize( m_window, &_width, &_height );
	bgfx::setViewRect( m_clearView, 0, 0, bgfx::BackbufferRatio::Equal );
}

int Window::createWindow( int _width, int _height, const char* _title )
{
	// ----------------------- glfw -------------------------- //

	WVDEBUG( "Creating Window [%i, %i]", _width, _height );

	if ( !glfwInit() )
	{
		WVFATAL( "GLFW could not initialize!" );
		glfwTerminate();
		return 0;
	}
	WVDEBUG( "GLFW Initialized" );

	m_window = glfwCreateWindow( _width, _height, _title, nullptr, nullptr );

	if ( !m_window )
	{
		WVFATAL( "Failed to create GLFW window" );
		glfwTerminate();
		return 0;
	}
	WVDEBUG( "GLFW Window created" );

	glfwMakeContextCurrent( m_window );

	hookEvents();

	// ----------------------- bgfx -------------------------- //

	WVDEBUG( "Setting up BGFX" );

	bgfx::renderFrame();

	bgfx::Init init;

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window( m_window );
#elif BX_PLATFORM_OSX
	init.platformData.nwh = glfwGetCocoaWindow( m_window );
#elif BX_PLATFORM_WINDOWS
	init.platformData.nwh = glfwGetWin32Window( m_window );
#endif

	init.type = bgfx::RendererType::Count;
	init.resolution.width = (uint32_t)_width;
	init.resolution.height = (uint32_t)_height;

	if ( !bgfx::init( init ) )
	{
		WVFATAL( "BGFX could not initialize!" );
		glfwTerminate();
		return 0;
	}

	bgfx::setViewClear( m_clearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x666699FF, 1.0f, 0 );
	bgfx::setViewRect( m_clearView, 0, 0, bgfx::BackbufferRatio::Equal );

	bgfx::RendererType::Enum backend = bgfx::getRendererType();
	const char* backendstrings[ 12 ] = { "Noop", "Agc", "Direct3D9", "Direct3D11", "Direct3D12", "Gnm", "Metal", "Nvn", "OpenGLES", "OpenGL", "Vulkan", "WebGPU" };
	WVDEBUG( "Selected backend %s", backendstrings[ backend ] );

	return 1;
}

int Window::pollEvents()
{
	if ( glfwWindowShouldClose( m_window ) )
	{
		WVDEBUG( "Closing..." );
		glfwTerminate();
		WVDEBUG( "GLFW Terminated" );
		return 0;
	}

	glfwPollEvents();


	return 1;
}

void Window::processInput()
{
	if ( glfwGetKey( m_window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) // escape exit
	{
		glfwSetWindowShouldClose( m_window, true );
	}
}


void Window::hookEvents()
{
	glfwSetKeyCallback( m_window, Window::handleKeyEvents );
	// glfwSetCursorPosCallback()
}

void WV::Window::setVSync( bool _value )
{
	m_vsync_enabled = _value;
	glfwSwapInterval( _value ? 1 : 0 );

}


void Window::handleApplicationEvents()
{

}

void Window::handleKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	Events::KeyEvent keyEvent( key );
	EventManager::call<Events::KeyEvent>( keyEvent );

	if ( action == GLFW_PRESS )
	{
		Events::KeyDownEvent downEvent( key );
		EventManager::call<Events::KeyDownEvent>( downEvent );
	}
	else if ( action == GLFW_RELEASE )
	{
		Events::KeyUpEvent upEvent( key );
		EventManager::call<Events::KeyUpEvent>( upEvent );
	}

}

void Window::handleMouseEvents( GLFWwindow* window, double xpos, double ypos )
{

}
