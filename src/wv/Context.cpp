#include "Context.h"

#include <stdio.h>

wv::Context::Context( ContextDesc* _desc )
{
	if ( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, _desc->graphicsApiVersion.major );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, _desc->graphicsApiVersion.minor );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	printf( "GLFW version: %s\n", glfwGetVersionString() );

	m_windowContext = glfwCreateWindow(_desc->width, _desc->height, _desc->name, NULL, NULL);
	if ( !m_windowContext )
	{
		fprintf( stderr, "Failed to create context\n" );
		return;
	}
	glfwMakeContextCurrent( m_windowContext );
}

void wv::Context::terminate()
{
	glfwTerminate();
}

wv::GraphicsDriverLoadProc wv::Context::getLoadProc()
{
	return (GraphicsDriverLoadProc)glfwGetProcAddress;
}

void wv::Context::beginFrame()
{
	// process input
	if ( glfwGetKey( m_windowContext, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
		glfwSetWindowShouldClose( m_windowContext, true );
}

void wv::Context::endFrame()
{
	glfwSwapBuffers( m_windowContext );
	glfwPollEvents();
}

bool wv::Context::isAlive()
{
	return !glfwWindowShouldClose( m_windowContext );
}
