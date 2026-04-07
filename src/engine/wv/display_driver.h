#pragma once

#include <stdint.h>

#include <wv/math/vector2.h>

namespace wv {

class DisplayDriver
{
public:
	DisplayDriver();
	~DisplayDriver();

	virtual bool initializeDisplay( uint16_t _width, uint16_t _height ) = 0;
	virtual void shutdown() = 0;

	virtual Vector2i getWindowSize( void )                   const { return { 0, 0 }; }
	virtual Vector2i getDisplaySize( int _displayIndex = 0 ) const { return { 0, 0 }; }

	virtual bool isMinimized() const { return false; }
	virtual bool isFocused()   const { return true; }

	virtual void setCursorLock( bool _lock ) { }
	virtual void setCursorVisible( bool _visible ) { }

protected:

	bool m_initialized = false;

};

}