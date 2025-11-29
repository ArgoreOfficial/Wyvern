#include "application.h"

#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <wv/math/math.h>
#include <cmath>

#include <string>
#include <fstream>
#include <sstream>

std::string loadTextFile( const std::string& _path )
{
	std::ifstream t( _path );
	std::stringstream buffer;
	buffer << t.rdbuf();

	return buffer.str();
}

void glfwErrorCallback( int error, const char* description )
{
	printf( "Error: %s\n", description );
}

bool cw::Application::initialize()
{
	if ( !glfwInit() )
		return false;

	glfwSetErrorCallback( glfwErrorCallback );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	m_windowWidth  = 900;
	m_windowHeight = 600;

	m_window = glfwCreateWindow( m_windowWidth, m_windowHeight, "Clockwork", NULL, NULL );
	if ( !m_window )
		return false;

	glfwMakeContextCurrent( m_window );

	if ( !m_renderer.setup( (GLADloadproc)glfwGetProcAddress ) )
		return false;

	glfwSwapInterval( 1 );

	m_camera = new wv::ICamera( wv::ICamera::kOrthographic, m_windowWidth, m_windowHeight );
	m_camera->getTransform().setPosition( { 0.0f, 0.0f, 0.0f } );
	m_camera->setOrthoWidth( 6.0f );

}

bool cw::Application::tick()
{
	if ( glfwWindowShouldClose( m_window ) )
		return false;

	double time = glfwGetTime();
	m_deltatime = time - m_time;
	m_time = time;

	render();

	glfwPollEvents();
	glfwSwapBuffers( m_window );

	return true;
}

void cw::Application::shutdown()
{
	glfwDestroyWindow( m_window );
	glfwTerminate();
}

void cw::Application::update()
{
	glfwGetFramebufferSize( m_window, &m_windowWidth, &m_windowHeight );
	m_camera->setPixelSize( (size_t)m_windowWidth, (size_t)m_windowHeight );

	//m_accumulator += m_deltatime;
	//while ( m_accumulator > m_fixed_delta_time )
	//{
	//	m_app->onFixedUpdate( m_fixed_delta_time );
	//
	//	m_fixed_runtime += m_fixed_delta_time;
	//	m_accumulator -= m_fixed_delta_time;
	//}

	//m_app->preUpdate();

	m_camera->update( m_deltatime );
	//m_app->onUpdate( m_deltatime );
	//m_sprite_renderer->update();

	//m_app->postUpdate();
}

void cw::Application::render()
{
	m_renderer.prepare( m_windowWidth, m_windowHeight );
	m_renderer.clear( std::sinf( m_time * 10.0f ) * 0.5f + 0.5f, 0.0f, 0.0f, 1.0f );

	// m_sprite_renderer->drawSprites();

	m_renderer.finalize();
}
