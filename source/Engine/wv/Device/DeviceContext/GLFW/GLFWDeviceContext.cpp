#include "GLFWDeviceContext.h"

#include <stdio.h>

#include <wv/Engine/Engine.h>
#include <wv/Events/InputListener.h>
#include <wv/Events/MouseListener.h>

#include <wv/Math/Vector2.h>

#ifdef WV_SUPPORT_IMGUI
#include <backends/imgui_impl_glfw.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_GLFW
void keyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods )
{
	wv::InputEvent inputEvent;
	inputEvent.buttondown = _action == GLFW_PRESS;
	inputEvent.buttonup = _action == GLFW_RELEASE;
	inputEvent.repeat = _action == GLFW_REPEAT;
	inputEvent.key = _key;
	inputEvent.scancode = _scancode;
	inputEvent.mods = _mods;

	wv::IInputListener::invoke( inputEvent );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_GLFW
void mouseCallback( GLFWwindow* window, double xpos, double ypos )
{
	wv::MouseEvent mouseEvent;

	mouseEvent.position = wv::Vector2i{ (int)xpos, (int)ypos };

	wv::Vector2i oldPos = wv::cEngine::get()->getMousePosition();
	mouseEvent.delta = wv::Vector2i{ (int)xpos - oldPos.x, (int)ypos - oldPos.y };
	
	wv::IMouseListener::invoke( mouseEvent );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_GLFW
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
#endif

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_GLFW
void onResizeCallback( GLFWwindow* window, int _width, int _height )
{
	wv::cEngine::get()->onResize( _width, _height );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_GLFW
void glfwErrorCallback(int _err, const char* _msg)
{
	const char* errmsg = "";

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

	wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "%i ::\n %s\n %s\n", _err, _msg, errmsg );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

wv::GLFWDeviceContext::GLFWDeviceContext()
{

}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::GLFWDeviceContext::initialize( ContextDesc* _desc )
{
#ifdef WV_SUPPORT_GLFW
	glfwSetErrorCallback( glfwErrorCallback );

	if ( !glfwInit() )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "Failed to initialize Device Context\n" );
		return false;
	}

	switch ( _desc->graphicsApi )
	{

	case WV_GRAPHICS_API_OPENGL:
	{
		int mj = _desc->graphicsApiVersion.major;
		int mn = _desc->graphicsApiVersion.minor;

		if ( mj < 3 || ( mj == 3 && mn < 2 ) ) // below 3.2 
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
	glfwWindowHint( GLFW_RESIZABLE, _desc->allowResize );

	Debug::Print( Debug::WV_PRINT_INFO, "Initialized Context Device\n" );
	Debug::Print( Debug::WV_PRINT_INFO, "  %s\n", glfwGetVersionString() );

	m_windowContext = glfwCreateWindow( _desc->width, _desc->height, _desc->name, NULL, NULL );

	glfwSetFramebufferSizeCallback( m_windowContext, onResizeCallback );
	glfwSetKeyCallback( m_windowContext, keyCallback );

	glfwSetCursorPosCallback( m_windowContext, mouseCallback );
	glfwSetMouseButtonCallback( m_windowContext, mouseButtonCallback );

	if ( !m_windowContext )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "Failed to create Context\n" );
		return false;
	}
	glfwMakeContextCurrent( m_windowContext );

	glfwGetWindowSize( m_windowContext, &m_width, &m_height );
    return true;
#else
	Debug::Print( Debug::WV_PRINT_FATAL, "GLFW is not supported\n" );
	return false;
#endif
}

void wv::GLFWDeviceContext::setSwapInterval( int _interval )
{
#ifdef WV_SUPPORT_GLFW
	glfwSwapInterval( _interval );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::terminate()
{
#ifdef WV_SUPPORT_GLFW
	glfwTerminate();
#endif
}

void wv::GLFWDeviceContext::initImGui()
{
#ifdef WV_SUPPORT_GLFW
#ifdef WV_SUPPORT_IMGUI
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL: case WV_GRAPHICS_API_OPENGL_ES1: case WV_GRAPHICS_API_OPENGL_ES2:
		ImGui_ImplGlfw_InitForOpenGL( m_windowContext, true );
		break;
	}
#endif
#endif
}

void wv::GLFWDeviceContext::terminateImGui()
{
#ifdef WV_SUPPORT_GLFW
#ifdef WV_SUPPORT_IMGUI
	ImGui_ImplGlfw_Shutdown();
#endif
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GraphicsDriverLoadProc wv::GLFWDeviceContext::getLoadProc()
{
#ifdef WV_SUPPORT_GLFW
	return (GraphicsDriverLoadProc)glfwGetProcAddress;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::pollEvents()
{
#ifdef WV_SUPPORT_GLFW
	// process input
	glfwPollEvents();

	m_alive = !glfwWindowShouldClose( m_windowContext );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::swapBuffers()
{
#ifdef WV_SUPPORT_GLFW
	glfwSwapBuffers( m_windowContext );

	// update frametime
	float t = static_cast<float>( m_time );
	m_time = glfwGetTime();
	m_deltaTime = m_time - t;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::onResize( int _width, int _height )
{
	iDeviceContext::onResize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::setSize( int _width, int _height )
{
	iDeviceContext::setSize( _width, _height );
#ifdef WV_SUPPORT_GLFW
	glfwSetWindowSize( m_windowContext, _width, _height );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::setMouseLock( bool _lock )
{
#ifdef WV_SUPPORT_GLFW
	glfwSetInputMode( m_windowContext, GLFW_CURSOR, _lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::setTitle( const char* _title )
{
#ifdef WV_SUPPORT_GLFW
	glfwSetWindowTitle( m_windowContext, _title );
#endif
}
