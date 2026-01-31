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

	Vector2i getWindowSize( void ) const { return m_workingAreaSize; }
	virtual wv::Vector2i getDisplaySize( int _displayIndex = 0 ) { return { 0, 0 }; }

	void resize( Vector2i _size ) {
		m_workingAreaSize = {
			Math::max( 1, _size.x ),
			Math::max( 1, _size.y )
		};
	}

	virtual bool isMinimized() const { return false; }

#ifdef WV_PLATFORM_WINDOWS
	virtual HWND winGetHandle() = 0;
#endif

protected:

	bool m_initialized = false;

	// Window Size
	wv::Vector2i m_workingAreaSize = { 900, 600 };

};

}