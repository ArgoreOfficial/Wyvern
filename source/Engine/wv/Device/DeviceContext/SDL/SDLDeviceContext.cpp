#include "SDLDeviceContext.h"

#include <stdio.h>

#include <wv/Engine/Engine.h>
#include <wv/Events/InputListener.h>
#include <wv/Events/MouseListener.h>
#include <wv/Events/WindowListener.h>

#include <wv/Math/Vector2.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#endif

#include <wv/Debug/Print.h>

#ifdef WV_SUPPORT_IMGUI
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_SDL2
void keyCallback( wv::iDeviceContext* _device, SDL_KeyboardEvent* _event )
{
	wv::sInputEvent inputEvent;
	inputEvent.buttondown = _event->type == SDL_KEYDOWN;
	inputEvent.buttonup = _event->type == SDL_KEYUP;
	inputEvent.repeat = _event->repeat != 0;
	inputEvent.scancode = _event->keysym.scancode;
	inputEvent.mods = _event->keysym.mod;

	inputEvent.key = _event->keysym.sym;
	if ( inputEvent.key >= 'a' && inputEvent.key <= 'z' )
		inputEvent.key -= 32;

	wv::iInputListener::invoke( inputEvent );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_SDL2
void mouseCallback( wv::iDeviceContext* _device, SDL_MouseMotionEvent* _event )
{
	wv::MouseEvent mouseEvent;
	mouseEvent.position.x = _event->x;
	mouseEvent.position.y = _event->y;
	mouseEvent.delta.x = _event->xrel;
	mouseEvent.delta.y = _event->yrel;
	
	wv::iMouseListener::invoke( mouseEvent );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_SDL2
void mouseButtonCallback( wv::iDeviceContext* _device, SDL_MouseButtonEvent* _event )
{
	wv::MouseEvent mouseEvent;

	SDL_GetMouseState( &mouseEvent.position.x, &mouseEvent.position.y );
	SDL_GetRelativeMouseState( &mouseEvent.delta.x, &mouseEvent.delta.y );

	switch ( _event->button )
	{
	case 1: mouseEvent.button = wv::MouseEvent::WV_MOUSE_BUTTON_LEFT;   break;
	case 3: mouseEvent.button = wv::MouseEvent::WV_MOUSE_BUTTON_RIGHT;  break;
	case 2: mouseEvent.button = wv::MouseEvent::WV_MOUSE_BUTTON_MIDDLE; break;
	}

	mouseEvent.buttondown = _event->type == SDL_MOUSEBUTTONDOWN;
	mouseEvent.buttonup   = _event->type == SDL_MOUSEBUTTONUP;

	wv::iMouseListener::invoke( mouseEvent );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////
#ifdef WV_SUPPORT_SDL2
void windowCallback( SDL_Window* _window, SDL_WindowEvent* _event )
{
	switch ( _event->event )
	{
		case SDL_WindowEventID::SDL_WINDOWEVENT_RESIZED: 
		{
			int w, h;
			SDL_GetWindowSize( _window, &w, &h );
			wv::cEngine::get()->onResize( w, h );

			wv::WindowEvent windowEvent;
			windowEvent.type = wv::WindowEvent::WV_WINDOW_RESIZED;
			windowEvent.size.x = w;
			windowEvent.size.y = h;

			wv::iWindowListener::invoke( windowEvent );
			break;
		}
		case SDL_WindowEventID::SDL_WINDOWEVENT_FOCUS_GAINED:
		{
			wv::WindowEvent windowEvent;
			windowEvent.type = wv::WindowEvent::WV_WINDOW_FOCUS_GAINED;
			wv::iWindowListener::invoke( windowEvent );
			break;
		}
		case SDL_WindowEventID::SDL_WINDOWEVENT_FOCUS_LOST:
		{
			wv::WindowEvent windowEvent;
			windowEvent.type = wv::WindowEvent::WV_WINDOW_FOCUS_LOST;
			wv::iWindowListener::invoke( windowEvent );
			break;
		}
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

wv::SDLDeviceContext::SDLDeviceContext() 
#ifdef WV_SUPPORT_SDL2
	: m_windowContext{ nullptr }
#endif
{

}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::SDLDeviceContext::initialize( ContextDesc* _desc )
{
#ifdef WV_SUPPORT_SDL2
	// glfwSetErrorCallback( glfwErrorCallback );

	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "Failed to initialize Device Context\n" );
		return false;
	}

	uint32_t flags = 0;

	switch ( _desc->graphicsApi )
	{

	case WV_GRAPHICS_API_OPENGL:
	{
		SDL_GL_LoadLibrary( NULL );

		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, _desc->graphicsApiVersion.major );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, _desc->graphicsApiVersion.minor );

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

		flags |= SDL_WINDOW_OPENGL;
	} break; // OpenGL 1.0 - 4.6

	case WV_GRAPHICS_API_OPENGL_ES1: case WV_GRAPHICS_API_OPENGL_ES2:
	{
		SDL_GL_LoadLibrary( NULL );

		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, _desc->graphicsApiVersion.major );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, _desc->graphicsApiVersion.minor );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );

		flags |= SDL_WINDOW_OPENGL;
	} break; // OpenGL ES 1 & 2

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
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context = emscripten_webgl_create_context( "#canvas", &attrs );
	emscripten_webgl_make_context_current( webgl_context );
#endif

	if ( _desc->allowResize ) flags |= SDL_WINDOW_RESIZABLE;

	SDL_SetHint( SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "1" );

	m_windowContext = SDL_CreateWindow( _desc->name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _desc->width, _desc->height, flags );
	
#ifdef WV_SUPPORT_OPENGL 
	m_glContext = SDL_GL_CreateContext( m_windowContext );
#endif

	SDL_version version;
	SDL_GetVersion( &version );
	Debug::Print( Debug::WV_PRINT_INFO, "Initialized Context Device\n" );
	Debug::Print( Debug::WV_PRINT_INFO, "  SDL %i.%i.%i\n", version.major, version.minor, version.patch );

	if ( !m_windowContext )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "Failed to create Context\n" );
		return false;
	}
	
	SDL_GetWindowSize( m_windowContext, &m_width, &m_height );

	return true;
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::setSwapInterval( int _interval )
{
#ifdef WV_SUPPORT_SDL2
	if ( SDL_GL_SetSwapInterval( _interval ) < 0 )
		Debug::Print( Debug::WV_PRINT_FATAL, "Failed to set VSync mode" );
#endif
}


///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::initImGui()
{
#ifdef WV_SUPPORT_SDL2
#ifdef WV_SUPPORT_IMGUI
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:
		ImGui_ImplOpenGL3_Init();
	case WV_GRAPHICS_API_OPENGL_ES1:
	case WV_GRAPHICS_API_OPENGL_ES2:
		ImGui_ImplSDL2_InitForOpenGL( m_windowContext, m_glContext );
		break;
	}
#endif
#endif
}

void wv::SDLDeviceContext::terminateImGui()
{
#ifdef WV_SUPPORT_SDL2
#ifdef WV_SUPPORT_IMGUI
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL:
		ImGui_ImplOpenGL3_Shutdown();
		break;
	}
	ImGui_ImplSDL2_Shutdown();
#endif
#endif
}


void wv::SDLDeviceContext::newImGuiFrame()
{
#ifdef WV_SUPPORT_SDL2
#ifdef WV_SUPPORT_IMGUI
	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL: case WV_GRAPHICS_API_OPENGL_ES1: case WV_GRAPHICS_API_OPENGL_ES2:
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		break;
	default:
		Debug::Print( Debug::WV_PRINT_FATAL, "SDL context newImGuiFrame() graphics mode not supported" );
		break;
	}
#endif
#endif
}


void wv::SDLDeviceContext::renderImGui()
{
#ifdef WV_SUPPORT_SDL2
#ifdef WV_SUPPORT_IMGUI
	ImGui::Render();

	switch ( m_graphicsApi )
	{
	case WV_GRAPHICS_API_OPENGL: case WV_GRAPHICS_API_OPENGL_ES1: case WV_GRAPHICS_API_OPENGL_ES2:
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
		break;
	default:
		Debug::Print( Debug::WV_PRINT_FATAL, "SDL context renderImGui() graphics mode not supported" );
		break;
	}
#endif
#endif
}


void wv::SDLDeviceContext::terminate()
{
#ifdef WV_SUPPORT_SDL2
	SDL_Quit();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GraphicsDriverLoadProc wv::SDLDeviceContext::getLoadProc()
{
#ifdef WV_SUPPORT_SDL2
	return (GraphicsDriverLoadProc)SDL_GL_GetProcAddress;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::pollEvents()
{
#ifdef WV_SUPPORT_SDL2
	// process input
	SDL_Event ev;
	while ( SDL_PollEvent( &ev ) )
	{
		switch ( ev.type )
		{
		case SDL_EventType::SDL_QUIT: m_alive = false; break;
		
		case SDL_EventType::SDL_KEYDOWN: case SDL_EventType::SDL_KEYUP: keyCallback( this, &ev.key ); break;

		case SDL_EventType::SDL_MOUSEBUTTONDOWN: mouseButtonCallback( this, &ev.button ); break;
		case SDL_EventType::SDL_MOUSEBUTTONUP:   mouseButtonCallback( this, &ev.button ); break;
		case SDL_EventType::SDL_MOUSEMOTION:     mouseCallback( this, &ev.motion ); break;

		case SDL_EventType::SDL_WINDOWEVENT: windowCallback( m_windowContext, &ev.window ); break;
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::swapBuffers()
{
#ifdef WV_SUPPORT_SDL2
	// if opengl
	SDL_GL_SwapWindow( m_windowContext );
	
	m_time = SDL_GetTicks64() / 1000.0;

	double last = m_performanceCounter;
	
	uint64_t freq = SDL_GetPerformanceFrequency();
	m_performanceCounter = static_cast<double>( SDL_GetPerformanceCounter() );

	if ( last == 0 )
		return;

	m_deltaTime = (double)( ( m_performanceCounter - last ) / (double)SDL_GetPerformanceFrequency() );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::onResize( int _width, int _height )
{
	iDeviceContext::onResize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::setSize( int _width, int _height )
{
	iDeviceContext::setSize( _width, _height );
#ifdef WV_SUPPORT_SDL2
	SDL_SetWindowSize( m_windowContext, _width, _height );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::setMouseLock( bool _lock )
{
#ifdef WV_SUPPORT_SDL2
	SDL_SetRelativeMouseMode( (SDL_bool)_lock );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SDLDeviceContext::setTitle( const char* _title )
{
#ifdef WV_SUPPORT_SDL2
	SDL_SetWindowTitle( m_windowContext, _title );
#endif
}
