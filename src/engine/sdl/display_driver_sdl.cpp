#ifdef WV_SUPPORT_SDL2

#include "display_driver_sdl.h"

#include <wv/application.h>
#include <wv/debug/log.h>

#include <SDL2/SDL.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#endif

wv::DisplayDriverSDL::DisplayDriverSDL()
{
	
}

bool wv::DisplayDriverSDL::initializeDisplay( uint16_t _width, uint16_t _height )
{
	if ( m_initialized )
	{
		WV_LOG_ERROR( "Display already initialized" );
		return false;
	}

	Application* app = Application::getSingleton();
	const std::string renderer = app->getGraphicsDriverName();

	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		WV_LOG_ERROR( "Failed to initialize Device Context\n" );
		return false;
	}

	uint32_t flags = 0;

#ifdef WV_SUPPORT_OPENGL
	if ( renderer == "opengl" ) // opengl 1.0 - 4.6
	{
		SDL_GL_LoadLibrary( NULL );

		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 6 );

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

		flags |= SDL_WINDOW_OPENGL;
	}
#endif

#ifdef WV_SUPPORT_OPENGLES
	if ( renderer == "gles" ) // opengl es 
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
#endif

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

	m_window_context = SDL_CreateWindow( "Wyvern", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, flags );

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

	m_initialized = true;

	return true;
}

void wv::DisplayDriverSDL::shutdown()
{
	SDL_DestroyWindow( m_window_context );
	SDL_Quit();
}

wv::Vector2i wv::DisplayDriverSDL::getWindowSize( void )
{
	if ( !m_window_context )
		return {};

	int width, height;
	SDL_GetWindowSize( m_window_context, &width, &height );

	return wv::Vector2i{ width, height };
}

wv::Vector2i wv::DisplayDriverSDL::getDisplaySize( int _displayIndex )
{
	SDL_Rect rect;
	if ( SDL_GetDisplayBounds( _displayIndex, &rect ) == 0 )
		return { rect.w, rect.h };

	return wv::Vector2i{ 0, 0 };
}

void wv::DisplayDriverSDL::swapBuffers()
{
	SDL_GL_SwapWindow( m_window_context );
}

void wv::DisplayDriverSDL::processEvents()
{
	Application* app = Application::getSingleton();

	SDL_Event ev;
	while ( SDL_PollEvent( &ev ) )
	{
		switch ( ev.type )
		{
		case SDL_EventType::SDL_QUIT: app->quit(); break;
		//case SDL_EventType::SDL_WINDOWEVENT: windowCallback( m_windowContext, &ev.window ); break;
		}
	}
}

uint64_t wv::DisplayDriverSDL::getTicks()
{
	return SDL_GetTicks64();
}

uint64_t wv::DisplayDriverSDL::getHighResolutionCounter()
{
	return SDL_GetPerformanceCounter();
}

uint64_t wv::DisplayDriverSDL::getHighResolutionFrequency()
{
	return SDL_GetPerformanceFrequency();
}

#endif