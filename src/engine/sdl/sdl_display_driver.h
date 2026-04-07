#ifdef WV_SUPPORT_SDL

#pragma once

#include <wv/display_driver.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef WV_PLATFORM_WINDOWS
#include <windows.h>
#endif

struct SDL_Window;
typedef struct SDL_GLContextState* SDL_GLContext;

namespace wv {

class SDLDisplayDriver : public DisplayDriver
{
public:
	SDLDisplayDriver();

	virtual bool initializeDisplay( uint16_t _width, uint16_t _height ) override;
	virtual void shutdown() override;

	virtual Vector2i getWindowSize( void ) const override;
	virtual Vector2i getDisplaySize( int _displayIndex = 0 ) const override;

	void swapBuffers();

	virtual bool isMinimized() const override;
	virtual bool isFocused()   const override;

	virtual void setCursorLock( bool _lock ) override;
	virtual void setCursorVisible( bool _visible ) override;
	
#ifdef WV_PLATFORM_WINDOWS
	HWND getWindowHandle();
#endif

	SDL_Window* getSDLWindowContext() const { return m_windowContext; }

private:

	SDL_Window* m_windowContext = nullptr;

#ifdef WV_SUPPORT_OPENGL 
	SDL_GLContext m_openglContext = nullptr;
#endif

};

}

#endif