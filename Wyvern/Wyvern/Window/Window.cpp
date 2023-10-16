#include "Window.h"

#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

using namespace WV;

Window::Window():
	m_window(nullptr)
{

}

Window::~Window()
{
	
}

void windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	
	bgfx::reset( (uint32_t)_width, (uint32_t)_height, BGFX_RESET_VSYNC );
	/*
	glfwGetWindowSize( window, &width, &height );
	bgfx::setViewRect( m_clearView, 0, 0, bgfx::BackbufferRatio::Equal );
	*/
	
}

int Window::createWindow()
{
	int width = 800;
	int height = 800;

	// ----------------------- glfw -------------------------- //

	WVDEBUG( "Creating Window..." );

	if ( !glfwInit() )
	{
		WVFATAL( "GLFW could not initialize!" );
		glfwTerminate();
		return 0;
	}
	WVDEBUG( "GLFW Initialized" );

	m_window = glfwCreateWindow( width, height, "Wyvern", nullptr, nullptr);

	if ( !m_window )
	{
		WVFATAL( "Failed to create GLFW window" );
		glfwTerminate();
		return 0;
	}
	WVDEBUG( "GLFW Window created" );

	glfwMakeContextCurrent( m_window );

	glfwSetFramebufferSizeCallback( m_window, windowResizeCallback );
	
	WVDEBUG( "Window Created [%i, %i]", width, height );



	// ----------------------- bgfx -------------------------- //

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

	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	
	if ( !bgfx::init( init ) )
	{
		WVFATAL( "BGFX could not initialize!" );
		glfwTerminate();
		return 0;
	}

	bgfx::setViewClear( m_clearView, BGFX_CLEAR_COLOR );
	bgfx::setViewRect( m_clearView, 0, 0, bgfx::BackbufferRatio::Equal );

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
