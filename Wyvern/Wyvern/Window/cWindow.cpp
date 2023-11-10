#include "cWindow.h"
#include <Wyvern/Managers/cEventManager.h>

using namespace WV;

cWindow::cWindow()
{

}

cWindow::~cWindow()
{

}

void cWindow::windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	glViewport( 0, 0, _width, _height );
	m_view_width = _width;
	m_view_height = _height;
}

int WV::cWindow::createWindow( const char* _title )
{
	int result = createWindow( 512, 512, _title );
	
	return result;
}


int cWindow::createWindow( int _width, int _height, const char* _title )
{
	// ----------------------- glfw -------------------------- //

	WV_DEBUG( "Creating Window [%i, %i]", _width, _height );

	if ( !glfwInit() )
	{
		WV_FATAL( "GLFW could not initialize!" );
		glfwTerminate();
		return 0;
	}
	WV_DEBUG( "GLFW Initialized" );

	m_window = glfwCreateWindow( _width, _height, _title, nullptr, nullptr );


	if ( !m_window )
	{
		WV_FATAL( "Failed to create GLFW window" );
		glfwTerminate();
		return 0;
	}
	WV_DEBUG( "GLFW Window created" );

	glfwMakeContextCurrent( m_window );
	gladLoadGL( glfwGetProcAddress );
	hookEvents();

	return 1;
}

int cWindow::pollEvents()
{
	if ( glfwWindowShouldClose( m_window ) )
	{
		WV_DEBUG( "Closing" );
		return 0;
	}

	glfwPollEvents();


	return 1;
}

void cWindow::processInput()
{
	if ( glfwGetKey( m_window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) // escape exit
	{
		glfwSetWindowShouldClose( m_window, true );
	}
}


void cWindow::hookEvents()
{
	glfwSetKeyCallback( m_window, cWindow::handleKeyEvents );
	// glfwSetCursorPosCallback()
}

void WV::cWindow::setVSync( bool _value )
{
	m_vsync_enabled = _value;
	glfwSwapInterval( _value ? 1 : 0 );

}

void WV::cWindow::setClearColour( uint32_t hex )
{
	// bgfx::setViewClear( m_clearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, hex, 1.0f, 0 );
}


void cWindow::handleApplicationEvents()
{

}

void cWindow::handleKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	Events::cKeyEvent keyEvent( key );
	cEventManager::call<Events::cKeyEvent>( keyEvent );

	if ( action == GLFW_PRESS )
	{
		Events::cKeyDownEvent downEvent( key );
		cEventManager::call<Events::cKeyDownEvent>( downEvent );
	}
	else if ( action == GLFW_RELEASE )
	{
		Events::cKeyUpEvent upEvent( key );
		cEventManager::call<Events::cKeyUpEvent>( upEvent );
	}

}

void cWindow::handleMouseEvents( GLFWwindow* window, double xpos, double ypos )
{

}
