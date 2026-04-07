#ifdef WV_SUPPORT_SDL

#include "sdl_display_driver.h"

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/platform/platform.h>
#include <wv/input/input_system.h>

#include <SDL3/SDL.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef WV_SUPPORT_IMGUI
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#endif

void wv::Platform::pollEvents( LowLevelInputQueue& _inputEventQueue )
{
	Application* app = Application::getSingleton();
	SDL_Event ev;

	
	SDLDisplayDriver* dd = (SDLDisplayDriver*)app->getDisplayDriver();
	bool isRelativeMotion = SDL_GetWindowRelativeMouseMode( dd->getSDLWindowContext() );

	if ( isRelativeMotion )
	{
		float x{}, y{};
		SDL_GetRelativeMouseState( &x, &y );
		if ( x != 0.0f || y != 0 )
		{
			_inputEventQueue.pushEvent(
				{
					.type = LowLevelInputQueue::WV_MOUSE_MOVE,
					.mouse = {
						.move = {
							.position = dd->getWindowSize() / 2,
							.delta = { (int)x, (int)y }
						}
					}
				} );
		}
	}

	while ( SDL_PollEvent( &ev ) )
	{
		bool handled = false;

	#ifdef WV_SUPPORT_IMGUI
		ImGui_ImplSDL3_ProcessEvent( &ev );
	#endif

		switch ( ev.type )
		{
		case SDL_EVENT_QUIT: 
			app->quit(); 
			break;

		case SDL_EVENT_MOUSE_MOTION:
		{
			if ( isRelativeMotion )
				break;

		#ifdef WV_SUPPORT_IMGUI
			if ( ImGui::GetIO().WantCaptureMouse )
				break;
		#endif

			_inputEventQueue.pushEvent(
				{
					.type = LowLevelInputQueue::WV_MOUSE_MOVE,
					.mouse = {
						.move = {
							.position = { (int)ev.motion.x,    (int)ev.motion.y },
							.delta    = { (int)ev.motion.xrel, (int)ev.motion.yrel }
						}
					}
				} );
		} break;

		case SDL_EVENT_MOUSE_WHEEL:
		{
		#ifdef WV_SUPPORT_IMGUI
			if ( ImGui::GetIO().WantCaptureMouse )
				break;
		#endif

			_inputEventQueue.pushEvent(
				{
					.type = LowLevelInputQueue::WV_MOUSE_SCROLL,
					.mouse = {
						.scroll = {
							.delta = ev.wheel.integer_y
						}
					}
				} );			
		} break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN: [[fallthrough]];
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
		#ifdef WV_SUPPORT_IMGUI
			if ( ImGui::GetIO().WantCaptureMouse )
				break;
		#endif

			_inputEventQueue.pushEvent(
				{
					.type = LowLevelInputQueue::WV_MOUSE_BUTTON,
					.mouse = {
						.button = {
							.index = ev.button.button,
							.state = ev.button.down
						}
					}
				} );
		} break;
		}
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

	if ( !SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) )
	{
		WV_LOG_ERROR( "Failed to initialize Device Context\n" );
		return false;
	}

	SDL_WindowFlags flags = 0;

#ifndef WV_DEBUG
	flags |= SDL_WINDOW_MAXIMIZED;
#endif
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

	m_windowContext = SDL_CreateWindow( "Wyvern", _width, _height, flags );

#ifdef WV_SUPPORT_OPENGL
	if( renderer == "opengl" || renderer == "gles" )
		m_openglContext = SDL_GL_CreateContext( m_windowContext );
#endif

	const int version = SDL_GetVersion();
	wv::Debug::Print( "Initialized Display Driver\n" );
	wv::Debug::Print( "  Renderer: %s\n", renderer.c_str() );
	wv::Debug::Print( 
		"  SDL %i.%i.%i\n", 
		SDL_VERSIONNUM_MAJOR( version ),
		SDL_VERSIONNUM_MINOR( version ),
		SDL_VERSIONNUM_MICRO( version )
	);

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

	int numDisplays;
	SDL_DisplayID* displays = SDL_GetDisplays( &numDisplays );

	WV_ASSERT( numDisplays > _displayIndex );
	
	if ( SDL_GetDisplayBounds( displays[ _displayIndex ], &rect ) )
		return { rect.w, rect.h };
	else
	{
		WV_LOG_ERROR( "[SDL3] %s\n", SDL_GetError() );
	}

	return wv::Vector2i{ 1920, 1080 };
}

void wv::SDLDisplayDriver::swapBuffers()
{
	SDL_GL_SwapWindow( m_windowContext );
}

bool wv::SDLDisplayDriver::isMinimized() const
{
	const SDL_WindowFlags flags = SDL_GetWindowFlags( m_windowContext );
	return flags & SDL_WINDOW_MINIMIZED;
}

bool wv::SDLDisplayDriver::isFocused() const
{
	const SDL_WindowFlags flags = SDL_GetWindowFlags( m_windowContext );
	return flags & SDL_WINDOW_INPUT_FOCUS;
}

void wv::SDLDisplayDriver::setCursorLock( bool _lock )
{
	SDL_SetWindowRelativeMouseMode( m_windowContext, _lock );
}

void wv::SDLDisplayDriver::setCursorVisible( bool _visible )
{
	if ( _visible )
		SDL_ShowCursor();
	else
		SDL_HideCursor();
}

#ifdef WV_PLATFORM_WINDOWS
HWND wv::SDLDisplayDriver::getWindowHandle()
{
	const SDL_PropertiesID propID = SDL_GetWindowProperties( m_windowContext );
	HWND hwnd = (HWND)SDL_GetPointerProperty( propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );
	
	WV_ASSERT( hwnd != NULL );

	return hwnd;
}
#endif

#endif