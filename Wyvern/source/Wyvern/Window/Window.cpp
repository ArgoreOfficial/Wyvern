#include "Window.h"
#include <Wyvern/Events/Events.h>

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

	int width = 800;
	int height = 800;


	WVDEBUG( "Creating Window..." );

	if ( glfwInit() == GLFW_FALSE )
	{
		WVFATAL( "GLFW could not initialize!" );
		glfwTerminate();
		return 0;
	}
	WVDEBUG( "GLFW Initialized" );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	m_window = glfwCreateWindow( width, height, "Wyvern", NULL, NULL );

	if ( m_window == NULL )
	{
		WVFATAL( "Failed to create GLFW window" );
		glfwTerminate();
		return 0;
	}
	WVDEBUG( "GLFW Window created" );


	glfwMakeContextCurrent( m_window );

	if ( glewInit() )
	{
		WVFATAL( "GLEW could not initialize!" );
		glfwTerminate();
		return 0;
	}
	WVDEBUG( "GLEW Initialized" );

	glViewport( 0, 0, width, height );
	glfwSetFramebufferSizeCallback( m_window, windowResizeCallback );
	
	WVDEBUG( "Window Created [%i, %i]", width, height );

	glEnable( GL_TEXTURE_2D );

	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	glDepthRange( 0.0f, 1.0f );


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

