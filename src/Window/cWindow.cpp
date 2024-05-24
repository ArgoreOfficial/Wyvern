#include "cWindow.h"

#include <stdio.h>


void processInput( GLFWwindow* window )
{
	if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
		glfwSetWindowShouldClose( window, true );
}


cWindow::cWindow()
{

}

cWindow::~cWindow()
{

}

bool cWindow::create( unsigned int _width, unsigned int _height, const char* _title )
{

	if ( !glfwInit() )
	{
		fprintf( stderr, "Failed to init GLFW\n" );
		return false;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	printf( "GLFW version: %s\n", glfwGetVersionString() );

	m_window_object = glfwCreateWindow( _width, _height, _title, NULL, NULL );
	if ( !m_window_object )
	{
		fprintf( stderr, "Failed to create GLFW window\n" );
		return false;
	}
	glfwMakeContextCurrent( m_window_object );

	m_width = _width;
	m_height = _height;

	return true;
}

void cWindow::startFrame( void )
{
	processInput( m_window_object );

}

void cWindow::endFrame( void )
{
	glfwSwapBuffers( m_window_object );
	glfwPollEvents();
}
