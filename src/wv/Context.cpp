#include "Context.h"

#include <stdio.h>

void glfwerrcb(int _err, const char* _msg)
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

wv::Context::Context( ContextDesc* _desc )
{
	glfwSetErrorCallback( glfwerrcb );
	
	if ( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return;
	}

	/// TODO: make configurable
#ifdef EMSCRIPTEN
	glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE );
#else
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, _desc->graphicsApiVersion.major );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, _desc->graphicsApiVersion.minor );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#endif

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
