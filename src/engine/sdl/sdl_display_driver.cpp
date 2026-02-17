#ifdef WV_SUPPORT_SDL2

#include "sdl_display_driver.h"

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/platform/platform.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#endif

void wv::Platform::pollEvents()
{
	Application* app = Application::getSingleton();
	SDL_Event ev;
	while ( SDL_PollEvent( &ev ) )
	{
		switch ( ev.type )
		{
		case SDL_QUIT: app->quit(); break;
		}

	#ifdef WV_SUPPORT_IMGUI
		ImGui_ImplSDL2_ProcessEvent( &ev );
	#endif
	}
}

wv::SDLDisplayDriver::SDLDisplayDriver()
{
	
}

bool wv::SDLDisplayDriver::initializeDisplay( uint16_t _width, uint16_t _height )
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

#ifdef WV_SUPPORT_VULKAN
	if ( renderer == "vulkan" ) // vk 1.3
	{
		flags |= SDL_WINDOW_VULKAN;
		flags |= SDL_WINDOW_RESIZABLE;
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

	m_windowContext = SDL_CreateWindow( "Wyvern", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, flags );

#ifdef WV_SUPPORT_OPENGL
	if( renderer == "opengl" || renderer == "gles" )
		m_openglContext = SDL_GL_CreateContext( m_windowContext );
#endif

	SDL_version version;
	SDL_GetVersion( &version );
	wv::Debug::Print( "Initialized Display Driver\n" );
	wv::Debug::Print( "  Renderer: %s\n", renderer.c_str() );
	wv::Debug::Print( "  SDL %i.%i.%i\n", version.major, version.minor, version.patch );

	if ( !m_windowContext )
	{
		WV_LOG_ERROR( "Failed to create Context\n" );
		return false;
	}

	SDL_GL_SetSwapInterval( 1 );

	m_initialized = true;

	return true;
}

void wv::SDLDisplayDriver::shutdown()
{
	SDL_DestroyWindow( m_windowContext );
	SDL_Quit();
}

wv::Vector2i wv::SDLDisplayDriver::getWindowSize( void ) const
{
	if ( !m_windowContext )
		return {};

	int width, height;
	SDL_GetWindowSize( m_windowContext, &width, &height );

	return wv::Vector2i{ width, height };
}

wv::Vector2i wv::SDLDisplayDriver::getDisplaySize( int _displayIndex ) const
{
	SDL_Rect rect;
	if ( SDL_GetDisplayBounds( _displayIndex, &rect ) == 0 )
		return { rect.w, rect.h };

	return wv::Vector2i{ 0, 0 };
}

void wv::SDLDisplayDriver::swapBuffers()
{
	SDL_GL_SwapWindow( m_windowContext );
}

bool wv::SDLDisplayDriver::isMinimized() const
{
	uint32_t flags = SDL_GetWindowFlags( m_windowContext );
	return flags & SDL_WINDOW_MINIMIZED;
}


#ifdef WV_PLATFORM_WINDOWS
HWND wv::SDLDisplayDriver::getWindowHandle()
{
	SDL_SysWMinfo wmInfo;
	SDL_VERSION( &wmInfo.version );
	SDL_GetWindowWMInfo( m_windowContext, &wmInfo );

	return wmInfo.info.win.window;
}
#endif

#endif