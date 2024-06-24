#pragma once

#include <SDL2/SDL.h>
#include <wv/Types.h>

#include <wv/Device/DeviceContext.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class SDLDeviceContext : public iDeviceContext
	{

	public:

		void terminate() override;

		GraphicsDriverLoadProc getLoadProc() override;

		void pollEvents() override;
		void swapBuffers() override;

		void onResize( int _width, int _height ) override;
		void setSize( int _width, int _height ) override;

		void setMouseLock( bool _lock ) override;
		void setTitle( const char* _title ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		friend class iDeviceContext;
		SDLDeviceContext( ContextDesc* _desc );
		
		SDL_Window* m_windowContext = nullptr;

		uint64_t m_performanceCounter = 0;

		// #ifdef WV_OPENGL_SUPPORTED
		SDL_GLContext m_glContext = nullptr;
		// #endif
	};
}