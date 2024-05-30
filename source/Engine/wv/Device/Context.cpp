#include "Context.h"

#include <stdio.h>

#include <wv/Application/Application.h>
#include <wv/Events/IInputListener.h>
#include <wv/Events/IMouseListener.h>

#include <wv/Math/Vector2.h>

// TODO: change events to iListeners, separate classes?
// iInputEventListener, iMouseEventListener, iWindowEventListener, iEventListener 
// iEventInvoker

void keyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods )
{
	/// TODO: move to application?
	if ( _key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS )
		glfwSetWindowShouldClose( _window, true );

	wv::InputEvent inputEvent;
	inputEvent.buttondown = _action == GLFW_PRESS;
	inputEvent.buttonup = _action == GLFW_RELEASE;
	inputEvent.repeat = _action == GLFW_REPEAT;
	inputEvent.key = _key;
	inputEvent.scancode = _scancode;
	inputEvent.mods = _mods;

	wv::IInputListener::invoke( inputEvent );
}

void mouseCallback( GLFWwindow* window, double xpos, double ypos )
{
	wv::MouseEvent mouseEvent;

	mouseEvent.position = wv::Vector2i{ (int)xpos, (int)ypos };
	
	wv::IMouseListener::invoke( mouseEvent );
}

void mouseButtonCallback( GLFWwindow* _window, int _button, int _action, int _mods )
{
	wv::MouseEvent mouseEvent;

	double xpos, ypos;
	glfwGetCursorPos( _window, &xpos, &ypos );
	mouseEvent.position = { (int)xpos, (int)ypos };

	switch ( _button )
	{
	case GLFW_MOUSE_BUTTON_LEFT:   mouseEvent.button = wv::MouseEvent::WV_MOUSE_BUTTON_LEFT;   break;
	case GLFW_MOUSE_BUTTON_RIGHT:  mouseEvent.button = wv::MouseEvent::WV_MOUSE_BUTTON_RIGHT;  break;
	case GLFW_MOUSE_BUTTON_MIDDLE: mouseEvent.button = wv::MouseEvent::WV_MOUSE_BUTTON_MIDDLE; break;
	}

	mouseEvent.buttondown = _action == GLFW_PRESS;
	mouseEvent.buttonup = _action == GLFW_RELEASE;

	wv::IMouseListener::invoke( mouseEvent );
}

void onResizeCallback( GLFWwindow* window, int _width, int _height )
{
	wv::Application::getApplication()->onResize( _width, _height );
}

void glfwErrorCallback(int _err, const char* _msg)
{
	const char* errmsg;

	switch( _err )
	{
		case GLFW_NO_ERROR:            errmsg = "No error has occurred."; break;
		case GLFW_NOT_INITIALIZED:     errmsg = "GLFW has not been initialized."; break;
		case GLFW_NO_CURRENT_CONTEXT:  errmsg = "No context is current for this thread."; break;
		case GLFW_INVALID_ENUM:        errmsg = "One of the arguments to the function was an invalid enum value."; break;
		case GLFW_INVALID_VALUE:       errmsg = "One of the arguments to the function was an invalid value."; break;
		case GLFW_OUT_OF_MEMORY:       errmsg = "A memory allocation failed."; break;
		case GLFW_API_UNAVAILABLE:     errmsg = "GLFW could not find support for the requested API on the system."; break;
		case GLFW_VERSION_UNAVAILABLE: errmsg = "The requested OpenGL or OpenGL ES version is not available."; break;
		case GLFW_PLATFORM_ERROR:      errmsg = "A platform-specific error occurred that does not match any of the more specific categories."; break;
		case GLFW_FORMAT_UNAVAILABLE:  errmsg = "The requested format is not supported or available."; break;
		case GLFW_NO_WINDOW_CONTEXT:   errmsg = "The specified window does not have an OpenGL or OpenGL ES context."; break;
		
	}

	fprintf( stderr, "%i ::\n %s\n %s\n", _err, _msg, errmsg );
}

wv::Context::Context( ContextDesc* _desc ):
	m_windowContext{ nullptr }
{
	glfwSetErrorCallback( glfwErrorCallback );
	
	if ( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize Device Context\n" );
		return;
	}

	switch ( _desc->graphicsApi )
	{

	case WV_GRAPHICS_API_OPENGL:
	{
		int mj = _desc->graphicsApiVersion.major;
		int mn = _desc->graphicsApiVersion.minor;

		if( mj < 3 || ( mj == 3 && mn < 2 ) ) // below 3.2 
			glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE );
		else
			glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	} break; // OpenGL 1.0 - 4.6

	case WV_GRAPHICS_API_OPENGL_ES1: case WV_GRAPHICS_API_OPENGL_ES2:
	{
		glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE );
	} break; // OpenGL ES 1 & 2

	}

	/// TEMPORARY---
	glfwWindowHint( GLFW_SAMPLES, 4 );
	/// ---TEMPORARY

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, _desc->graphicsApiVersion.major );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, _desc->graphicsApiVersion.minor );

	printf( "Initialized Context Device\n" );
	printf( "  %s\n", glfwGetVersionString() );

	m_windowContext = glfwCreateWindow(_desc->width, _desc->height, _desc->name, NULL, NULL);
	
	glfwSetFramebufferSizeCallback( m_windowContext, onResizeCallback );
	glfwSetKeyCallback( m_windowContext, keyCallback );

	glfwSetCursorPosCallback( m_windowContext, mouseCallback );
	glfwSetMouseButtonCallback( m_windowContext, mouseButtonCallback );

	if ( !m_windowContext )
	{
		fprintf( stderr, "Failed to create Context\n" );
		return;
	}
	glfwMakeContextCurrent( m_windowContext );

	glfwSwapInterval( 1 );

	glfwGetWindowSize( m_windowContext, &m_width, &m_height );
}

void wv::Context::terminate()
{
	glfwTerminate();
}

wv::GraphicsDriverLoadProc wv::Context::getLoadProc()
{
	return (GraphicsDriverLoadProc)glfwGetProcAddress;
}

void wv::Context::pollEvents()
{
	// process input
	glfwPollEvents();
}

void wv::Context::swapBuffers()
{
	glfwSwapBuffers( m_windowContext );

	// update frametime
	float t = m_time;
	m_time = glfwGetTime();
	m_frameTime = m_time - t;
}

void wv::Context::onResize( int _width, int _height )
{
	m_width  = _width;
	m_height = _height;
}

bool wv::Context::isAlive()
{
	return !glfwWindowShouldClose( m_windowContext );
}

float wv::Context::getAspect()
{
	if ( m_width == 0 || m_height == 0 )
		return 1.0f;

	return (float)m_width / (float)m_height;
}

void wv::Context::setMouseLock( bool _lock )
{
	glfwSetInputMode( m_windowContext, GLFW_CURSOR, _lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
}