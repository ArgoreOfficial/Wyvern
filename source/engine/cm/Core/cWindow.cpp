#include "cWindow.h"

#include <wv/Core/cApplication.h>

#include <stdio.h>

void keyCallback( GLFWwindow* _window, int _key, int _scancode, int _action, int _mods )
{
	sInputInfo* info = new sInputInfo();
	info->type = sInputInfo::InputInfo_Key;

	info->buttondown     = _action == GLFW_PRESS;
	info->buttonup       = _action == GLFW_RELEASE;
	info->repeat         = _action == GLFW_REPEAT;
	info->key            = _key;
	info->scancode       = _scancode;
	info->mods           = _mods;
	info->mouse_position = { 0, 0 };

	wv::cApplication::getInstance().onRawInput( info );

	delete info;
}

void mouseCallback( GLFWwindow* window, double xpos, double ypos )
{
	sInputInfo* info = new sInputInfo();
	info->type = sInputInfo::InputInfo_Mouse;

	info->buttondown = false;
	info->buttonup   = false;
	info->repeat     = false;
	info->key        = 0;
	info->scancode   = 0;
	info->mods       = 0;

	wv::cVector2i new_pos = wv::cVector2i{ (int)xpos, (int)ypos };
	info->mouse_position = new_pos;
	
	wv::cApplication::getInstance().onRawInput( info );

	delete info;
}

void onResizeCallback( GLFWwindow* window, int _width, int _height )
{
	wv::cApplication::getInstance().onResize( _width, _height );
}

cm::cWindow::cWindow()
{

}

cm::cWindow::~cWindow()
{

}

int cm::cWindow::create( unsigned int _width, unsigned int _height, const char* _title )
{
	m_width = _width;
	m_height = _height;
	m_title = _title;
	
	if ( !glfwInit() )
	{
		fprintf( stderr, "Failed to init GLFW\n" );
		return 1;
	}

	applyWindowAttributes( false );
	createWindow();
	
	return 0;
}

void cm::cWindow::destroy( void )
{
	glfwTerminate();
}

void cm::cWindow::processInput( void )
{
	if ( glfwGetKey( m_window_object, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
		glfwSetWindowShouldClose( m_window_object, true );

	double xpos, ypos;
	glfwGetCursorPos( m_window_object, &xpos, &ypos );
	m_mouse_pos = wv::cVector2i{ (int)xpos, (int)ypos };
}

void cm::cWindow::display( void )
{
	glfwSwapBuffers( m_window_object );
	
	
	glfwPollEvents();
}

float cm::cWindow::getAspect()
{
	if ( m_width == 0 || m_height == 0 )
		return 1.0f;

	return (float)m_width / (float)m_height;
}

static int getWindowHint_glfw( cm::eWindowAttribute _attribute )
{
	switch ( _attribute )
	{
	case cm::WindowAttribute_Decorated:    return GLFW_DECORATED; break;
	case cm::WindowAttribute_ClickThrough: return GLFW_MOUSE_PASSTHROUGH; break;
	case cm::WindowAttribute_Transparent:  return GLFW_TRANSPARENT_FRAMEBUFFER; break;
	case cm::WindowAttribute_Focused:      return GLFW_FOCUSED; break;
	case cm::WindowAttribute_FocusOnShow:  return GLFW_FOCUS_ON_SHOW; break;
	case cm::WindowAttribute_AlwaysOnTop:  return GLFW_FLOATING; break;
	case cm::WindowAttribute_Resizable:    return GLFW_RESIZABLE; break;
	case cm::WindowAttribute_Maximized:    return GLFW_MAXIMIZED; break;
	case cm::WindowAttribute_Minimized:    return GLFW_ICONIFIED; break;

	default: return GLFW_FALSE;
	}
}

void cm::cWindow::setWindowAttribute( eWindowAttribute _attribute, int _value )
{
	m_window_attributes[ _attribute ] = _value;
}

void cm::cWindow::applyWindowAttributes( bool _recreate_window )
{
	for ( int i = 0; i < eWindowAttribute_SIZE; i++ )
		glfwWindowHint( getWindowHint_glfw( (eWindowAttribute)i ), m_window_attributes[ i ] );
	
	if ( !_recreate_window )
		return;

	glfwDestroyWindow( m_window_object );
	createWindow();

}

void cm::cWindow::createWindow()
{
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	printf( "GLFW version: %s\n", glfwGetVersionString() );

	GLFWmonitor* fullscreen_monitor = nullptr;
	if ( m_window_attributes[ WindowAttribute_Fullscreen ] == 1 )
		fullscreen_monitor = glfwGetPrimaryMonitor();

	m_window_object = glfwCreateWindow( m_width, m_height, m_title, nullptr, nullptr );

	// error handling
	if ( !m_window_object )	{ fprintf( stderr, "Failed to create GLFW window\n" ); return; }

	glfwMakeContextCurrent( m_window_object );

	glfwSetFramebufferSizeCallback( m_window_object, onResizeCallback );
	glfwSetKeyCallback( m_window_object, keyCallback );

	glfwSetInputMode( m_window_object, GLFW_CURSOR, GLFW_CURSOR_DISABLED ); // center cursor, TODO: allow to be disabled
	glfwSetCursorPosCallback( m_window_object, mouseCallback );
}

void cm::cWindow::onResize( int _width, int _height )
{
	m_width = _width;
	m_height = _height;
}
