#include "application.h"

#include <wv/math/math.h>
#include <wv/debug/log.h>
#include <wv/memory/memory.h>

#include <windows/windows_file_system.h>

#ifdef WV_SUPPORT_SDL2
#include <sdl/display_driver_sdl.h>
#endif

#include <cmath>
#include <stdio.h>

wv::Application* wv::Application::singleton = nullptr;

void glfwErrorCallback( int error, const char* description )
{
	printf( "Error: %s\n", description );
}

wv::Application::Application()
{
	singleton = this;
}

bool wv::Application::initialize( int _windowWidth, int _windowHeight )
{
	m_graphicsDriverName = "opengl"; // TODO

	m_displayDriver = WV_NEW( DisplayDriverSDL );
	
	if ( !m_displayDriver->initializeDisplay( _windowWidth, _windowHeight ) )
	{
		WV_FREE( m_displayDriver );
		return false;
	}

	if ( !m_renderer.setup() )
		return false;

	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	m_camera = new wv::ICamera( wv::ICamera::kOrthographic, windowSize.x, windowSize.y );
	m_camera->getTransform().setPosition( { 0.0f, 0.0f, 0.0f } );
	m_camera->setOrthoWidth( 6.0f );

	m_filesystem = new WindowsFileSystem();
	m_filesystem->addDirectory( "./dat/" );

	///////////////////////////////////////////////////////////////////////////

	std::string vs = m_filesystem->loadString( "debug_vs.glsl" );
	std::string fs = m_filesystem->loadString( "debug_fs.glsl" );

	m_debugPipeline = m_renderer.createPipeline( vs.c_str(), fs.c_str() );
}

bool wv::Application::tick()
{
	if ( !m_alive )
		return false;

	m_displayDriver->swapBuffers();
	m_displayDriver->processEvents();

	// update runtime and deltatime
	
	uint64_t ticks = m_displayDriver->getHighResolutionCounter();
	m_runtime = m_displayDriver->getTicks() / 1000.0;
	m_deltatime = (double)( ( ticks - m_lastTicks ) / (double)m_displayDriver->getHighResolutionFrequency() );

	render();


	m_lastTicks = ticks;
	return true;
}

void wv::Application::shutdown()
{
	m_displayDriver->shutdown();
	
}

void wv::Application::update()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();
	
	m_camera->setPixelSize( (size_t)windowSize.x, (size_t)windowSize.y );

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

void wv::Application::render()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	m_renderer.prepare( windowSize.x, windowSize.y );
	m_renderer.clear( std::sinf( m_runtime * 10.0f ) * 0.5f + 0.5f, 0.0f, 0.0f, 1.0f );

	// m_sprite_renderer->drawSprites();

	m_renderer.finalize();
}
