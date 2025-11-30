#ifdef WV_SUPPORT_SDL2

#pragma once

#include <wv/display_driver.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

struct SDL_Window;
typedef void* SDL_GLContext;

namespace wv {

class DisplayDriverSDL : public wv::DisplayDriver
{
public:
	DisplayDriverSDL();

	virtual bool initializeDisplay( uint16_t _width, uint16_t _height ) override;
	virtual void shutdown() override;

	virtual wv::Vector2i getWindowSize( void ) override;
	virtual wv::Vector2i getDisplaySize( int _displayIndex = 0 ) override;

	virtual void swapBuffers() override;
	virtual void processEvents() override;

	virtual uint64_t getTicks() override;
	virtual uint64_t getHighResolutionCounter() override;
	virtual uint64_t getHighResolutionFrequency() override;
	
private:

	SDL_Window* m_window_context = nullptr;

#ifdef WV_SUPPORT_OPENGL 
	SDL_GLContext m_opengl_context = nullptr;
#endif

};

}

#endif