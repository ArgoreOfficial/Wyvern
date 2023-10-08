#include "Window.h"
using namespace WV;

Window::Window():
	m_window(nullptr)
{

}

Window::~Window()
{
	glfwTerminate();
}

void windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	glViewport( 0, 0, _width, _height );
	// WVDEBUG( "Window resized: %i, %i", _width, _height );
}

int Window::createWindow()
{
	WVDEBUG( "Creating window..." );

	if ( glfwInit() == GLFW_FALSE )
	{
		WVFATAL( "glfw could not initialize!" );
		glfwTerminate();
		return 0;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );


	m_window = glfwCreateWindow( 800, 600, "Wyvern", NULL, NULL );

	if ( m_window == NULL )
	{
		WVFATAL( "Failed to create GLFW window" );
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent( m_window );

	if ( glewInit() )
	{
		WVFATAL( "glew could not initialize!" );
		glfwTerminate();
		return 0;
	}

	glViewport( 0, 0, 800, 600 );
	glfwSetFramebufferSizeCallback( m_window, windowResizeCallback );
	
	WVDEBUG( "Window Created" );
	
	return 1;
}

int WV::Window::pollEvents()
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

void WV::Window::processInput()
{
	if ( glfwGetKey( m_window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) // escape exit
	{
		glfwSetWindowShouldClose( m_window, true );
	}
}
