#include "application.h"

#ifdef WV_SUPPORT_SDL2
#include <SDL2/SDL.h>
#endif

#include <wv/math/math.h>
#include <wv/debug/log.h>

#include <cmath>
#include <stdio.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#endif

void glfwErrorCallback( int error, const char* description )
{
	printf( "Error: %s\n", description );
}

bool wv::Application::initialize( int _windowWidth, int _windowHeight )
{
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		WV_LOG_ERROR( "Failed to initialize Device Context\n" );
		return false;
	}

	const bool gles = false;

	uint32_t flags = 0;

	if ( gles ) // opengl es 
	{
		SDL_GL_LoadLibrary( NULL );

		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

		flags |= SDL_WINDOW_OPENGL;
	}
	else // opengl 1.0 - 4.6
	{
		SDL_GL_LoadLibrary( NULL );

		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 6 );

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

		flags |= SDL_WINDOW_OPENGL;
	}

#ifdef EMSCRIPTEN
	EmscriptenWebGLContextAttributes attrs;
	attrs.antialias = true;
	attrs.majorVersion = 3;
	attrs.minorVersion = 0;
	attrs.alpha = true;
	attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;

	// The following lines must be done in exact order, or it will break!
	emscripten_webgl_init_context_attributes( &attrs ); // you MUST init the attributes before creating the context
	attrs.majorVersion = 3; // you MUST set the version AFTER the above line
	attrs.minorVersion = 0;
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context = emscripten_webgl_create_context( "#canvas", &attrs );
	emscripten_webgl_make_context_current( webgl_context );
#endif

	//SDL_GL_CONTEXT_DEBUG_FLAG
	//glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true );
	//glfwWindowHint( GLFW_RESIZABLE, false );

	m_window_context = SDL_CreateWindow( "Wyvern", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _windowWidth, _windowHeight, flags );

#ifdef WV_SUPPORT_OPENGL 
	m_opengl_context = SDL_GL_CreateContext( m_window_context );
#endif

	SDL_version version;
	SDL_GetVersion( &version );
	wv::Debug::Print( "Initialized Context Device\n" );
	wv::Debug::Print( "  SDL %i.%i.%i\n", version.major, version.minor, version.patch );

	if ( !m_window_context )
	{
		WV_LOG_ERROR( "Failed to create Context\n" );
		return false;
	}

	SDL_GL_SetSwapInterval( 1 );

	if ( !m_renderer.setup( (GLADloadproc)SDL_GL_GetProcAddress ) )
		return false;

	SDL_GetWindowSize( m_window_context, &m_windowWidth, &m_windowHeight );

	m_camera = new wv::ICamera( wv::ICamera::kOrthographic, m_windowWidth, m_windowHeight );
	m_camera->getTransform().setPosition( { 0.0f, 0.0f, 0.0f } );
	m_camera->setOrthoWidth( 6.0f );

}

bool wv::Application::tick()
{
	if ( !m_alive )
		return false;

	SDL_GL_SwapWindow( m_window_context );

	SDL_Event ev;
	while ( SDL_PollEvent( &ev ) )
	{
		switch ( ev.type )
		{
		case SDL_EventType::SDL_QUIT: m_alive = false; break;
		//case SDL_EventType::SDL_WINDOWEVENT: windowCallback( m_windowContext, &ev.window ); break;
		}
	}

	// update runtime and deltatime
	{
		m_runtime = SDL_GetTicks64() / 1000.0;
		double last = m_performance_counter;

		uint64_t freq = SDL_GetPerformanceFrequency();
		m_performance_counter = static_cast<double>( SDL_GetPerformanceCounter() );

		if ( last == 0 )
			return true;

		m_deltatime = (double)( ( m_performance_counter - last ) / (double)SDL_GetPerformanceFrequency() );
	}

	render();

	return true;
}

void wv::Application::shutdown()
{
	SDL_DestroyWindow( m_window_context );
	SDL_Quit();
}

void wv::Application::update()
{
	SDL_GetWindowSize( m_window_context, &m_windowWidth, &m_windowHeight );

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

void wv::Application::render()
{
	m_renderer.prepare( m_windowWidth, m_windowHeight );
	m_renderer.clear( std::sinf( m_runtime * 10.0f ) * 0.5f + 0.5f, 0.0f, 0.0f, 1.0f );

	// m_sprite_renderer->drawSprites();

	m_renderer.finalize();
}
