#include "cWindow.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <string>
#include <format>

cm::cWindow::cWindow( void )
{
}

cm::cWindow::~cWindow( void )
{
}

void cm::cWindow::create( void )
{
	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	m_window = glfwCreateWindow( m_width, m_height, "Vulkan", nullptr, nullptr );
}

void cm::cWindow::update( void )
{
	glfwPollEvents();

	double current_time = glfwGetTime();
	double delta_time = current_time - m_last_time;
	m_last_time = current_time;

	std::string title = std::format( "Vulkan :: FPS: {}", ( 1.0 / delta_time ) );
	glfwSetWindowTitle( m_window, title.c_str() );
}

void cm::cWindow::destroy( void )
{
	glfwDestroyWindow( m_window );
	glfwTerminate();
}

void cm::cWindow::createSurface( VkInstance _instance, VkSurfaceKHR* _surface )
{
	glfwCreateWindowSurface( _instance, m_window, nullptr, _surface );
}

HWND cm::cWindow::getWin32Window( void )
{
	return HWND();
}

const char** cm::cWindow::getRequiredInstanceExtensions( uint32_t* _count )
{
	return glfwGetRequiredInstanceExtensions( _count );
}

void cm::cWindow::getFramebufferSize( int* _out_width, int* _out_height )
{
	glfwGetFramebufferSize( m_window, _out_width, _out_height );
}

bool cm::cWindow::shouldClose( void )
{
	return glfwWindowShouldClose( m_window );
}
