#ifdef WV_SUPPORT_GLFW

#include "GLFWDeviceContext.h"

#include <stdio.h>

#include <wv/Engine/Engine.h>
#include <wv/Events/Events.h>

#include <wv/Math/Vector2.h>

#include <iostream>

#ifdef WV_SUPPORT_IMGUI
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

void keyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods )
{
	wv::sInputEvent inputEvent;
	inputEvent.buttondown = _action == GLFW_PRESS;
	inputEvent.buttonup = _action == GLFW_RELEASE;
	inputEvent.repeat = _action == GLFW_REPEAT;
	inputEvent.key = wv::eKey::WV_KEY_UNKNOWN;
	inputEvent.scancode = _scancode;
	inputEvent.mods = _mods;

	wv::cInputEventDispatcher::post( inputEvent );
}

///////////////////////////////////////////////////////////////////////////////////////

void mouseCallback( GLFWwindow* window, double xpos, double ypos )
{
	wv::sMouseEvent mouseEvent;

	mouseEvent.position = wv::Vector2i{ (int)xpos, (int)ypos };

	wv::Vector2i oldPos = wv::cEngine::get()->getMousePosition();
	mouseEvent.delta = wv::Vector2i{ (int)xpos - oldPos.x, (int)ypos - oldPos.y };
	
	wv::cMouseEventDispatcher::post( mouseEvent );
}

///////////////////////////////////////////////////////////////////////////////////////

void mouseButtonCallback( GLFWwindow* _window, int _button, int _action, int _mods )
{
	wv::sMouseEvent mouseEvent;

	double xpos, ypos;
	glfwGetCursorPos( _window, &xpos, &ypos );
	mouseEvent.position = { (int)xpos, (int)ypos };

	switch ( _button )
	{
	case GLFW_MOUSE_BUTTON_LEFT:   mouseEvent.button = wv::sMouseEvent::WV_MOUSE_BUTTON_LEFT;   break;
	case GLFW_MOUSE_BUTTON_RIGHT:  mouseEvent.button = wv::sMouseEvent::WV_MOUSE_BUTTON_RIGHT;  break;
	case GLFW_MOUSE_BUTTON_MIDDLE: mouseEvent.button = wv::sMouseEvent::WV_MOUSE_BUTTON_MIDDLE; break;
	}
	
	mouseEvent.buttondown = _action == GLFW_PRESS;
	mouseEvent.buttonup = _action == GLFW_RELEASE;

	wv::cMouseEventDispatcher::post( mouseEvent );
}

///////////////////////////////////////////////////////////////////////////////////////

void windowFocusCallback(GLFWwindow* _window, int _focused)
{
	wv::sWindowEvent windowEvent;
	windowEvent.type = _focused == GLFW_TRUE
		? wv::sWindowEvent::WV_WINDOW_FOCUS_GAINED
		: wv::sWindowEvent::WV_WINDOW_FOCUS_LOST;

	wv::cWindowEventDispatcher::post( windowEvent );
}

///////////////////////////////////////////////////////////////////////////////////////

void onResizeCallback( GLFWwindow* window, int _width, int _height )
{
	wv::cEngine::get()->onResize( _width, _height );

	wv::sWindowEvent windowEvent;
	windowEvent.type = wv::sWindowEvent::WV_WINDOW_RESIZED;
	windowEvent.size.x = _width;
	windowEvent.size.y = _height;

	wv::cWindowEventDispatcher::post( windowEvent );
}

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

bool wv::GLFWDeviceContext::initialize( ContextDesc* _desc )
{
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
	if ( !m_windowContext )
	{
		Debug::Print(Debug::WV_PRINT_FATAL, "Failed to create Context\n");
		return false;
	}

	glfwSetFramebufferSizeCallback( m_windowContext, onResizeCallback );
	glfwSetKeyCallback( m_windowContext, keyCallback );

	glfwSetCursorPosCallback( m_windowContext, mouseCallback );
	glfwSetMouseButtonCallback( m_windowContext, mouseButtonCallback );

	glfwSetWindowFocusCallback( m_windowContext, windowFocusCallback );

	glfwMakeContextCurrent( m_windowContext );

	glfwGetWindowSize( m_windowContext, &m_width, &m_height );

    return true;
}

void wv::GLFWDeviceContext::setSwapInterval( int _interval )
{
	glfwSwapInterval( _interval );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::terminate()
{
	glfwTerminate();
}

void wv::GLFWDeviceContext::initImGui()
{
#ifdef WV_SUPPORT_IMGUI
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:
		ImGui_ImplOpenGL3_Init();
	case WV_GRAPHICS_API_OPENGL_ES1:
	case WV_GRAPHICS_API_OPENGL_ES2:
		ImGui_ImplGlfw_InitForOpenGL( m_windowContext, true );
		break;
	}
#endif
}

void wv::GLFWDeviceContext::terminateImGui()
{
#ifdef WV_SUPPORT_IMGUI
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:
		ImGui_ImplOpenGL3_Shutdown();
		break;
	}
	ImGui_ImplGlfw_Shutdown();
#endif
}

bool wv::GLFWDeviceContext::newImGuiFrame()
{
#ifdef WV_SUPPORT_IMGUI
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL: case WV_GRAPHICS_API_OPENGL_ES1: case WV_GRAPHICS_API_OPENGL_ES2:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		return true;
		break;
	default:
		Debug::Print( Debug::WV_PRINT_FATAL, "GLFW context newImGuiFrame() graphics mode not supported" );
	}
#endif

	return false;
}

void wv::GLFWDeviceContext::renderImGui()
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::Render();

	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL: case WV_GRAPHICS_API_OPENGL_ES1: case WV_GRAPHICS_API_OPENGL_ES2:
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
		break;
	default:
		Debug::Print( Debug::WV_PRINT_FATAL, "GLFW context renderImGui() graphics mode not supported" );
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GraphicsDriverLoadProc wv::GLFWDeviceContext::getLoadProc()
{
	return (GraphicsDriverLoadProc)glfwGetProcAddress;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::pollEvents()
{
	// process input
	glfwPollEvents();

	if ( glfwWindowShouldClose( m_windowContext ) )
		m_alive = false;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::swapBuffers()
{
	glfwSwapBuffers( m_windowContext );

	// update frametime
	float t = static_cast<float>( m_time );
	m_time = glfwGetTime();
	m_deltaTime = m_time - t;
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
	glfwSetWindowSize( m_windowContext, _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::setMouseLock( bool _lock )
{
	glfwSetInputMode( m_windowContext, GLFW_CURSOR, _lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::GLFWDeviceContext::setTitle( const char* _title )
{
	glfwSetWindowTitle( m_windowContext, _title );
}

#endif