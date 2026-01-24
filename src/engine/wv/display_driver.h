#pragma once

#include <stdint.h>

#include <wv/math/vector2.h>

#ifdef WV_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace wv {

class DisplayDriver
{
public:
	DisplayDriver();
	~DisplayDriver();

	virtual bool initializeDisplay( uint16_t _width, uint16_t _height ) = 0;
	virtual void shutdown() = 0;

	virtual wv::Vector2i getWindowSize ( void ) { return { 0, 0 }; }
	virtual wv::Vector2i getDisplaySize( int _displayIndex = 0 ) { return { 0, 0 }; }

	virtual void swapBuffers() { /* no-op */ }
	virtual void processEvents() { /* no-op */ }

	virtual uint64_t getTicks() { return 0; }
	virtual uint64_t getHighResolutionCounter() { return 0; }
	virtual uint64_t getHighResolutionFrequency() { return 1; }
	
	virtual bool isMinimized() const { return false; }

#ifdef WV_PLATFORM_WINDOWS
	virtual HWND winGetHandle() = 0;
#endif

protected:

	bool m_initialized = false;

};

}