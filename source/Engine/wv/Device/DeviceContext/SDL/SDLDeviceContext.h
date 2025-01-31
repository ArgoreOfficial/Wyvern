#pragma once

#ifdef WV_SUPPORT_SDL2
#include <SDL2/SDL.h>
#endif
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
		virtual void initImGui() override;
		virtual void terminateImGui() override;
		virtual void newImGuiFrame() override;
		virtual void renderImGui() override;

		void terminate() override;

		GraphicsDriverLoadProc getLoadProc() override;

		void pollEvents() override;
		void swapBuffers() override;

		void onResize( int _width, int _height ) override;
		void setSize( int _width, int _height ) override;

		void setMouseLock( bool _lock ) override;
		void setTitle( const char* _title ) override;

		void setSwapInterval( int _interval ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		friend class iDeviceContext;
		SDLDeviceContext();
		
		bool initialize( ContextDesc* _desc ) override;

	#ifdef WV_SUPPORT_SDL2
		SDL_Window* m_windowContext = nullptr;
	#ifdef WV_SUPPORT_OPENGL 
		SDL_GLContext m_glContext = nullptr;
	#endif
	#endif
		uint64_t m_performanceCounter = 0;


	};
}