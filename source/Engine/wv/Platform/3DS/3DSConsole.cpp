#ifdef WV_PLATFORM_3DS

#include "3DSConsole.h"

#include <wv/Platform/3DS/3DS.h>

#include <3ds.h>
#include <citro3d.h>

#include <stdio.h>

void wv::Console::initialize()
{

	/// TOOD: move to platform specific
	// platform init
	gfxInitDefault();
	C3D_Init( C3D_DEFAULT_CMDBUF_SIZE );
	// console init
	consoleInit( GFX_BOTTOM, 0 );

	wv::Console::Internal::g_initialized = true;
}

void wv::Console::setForegroundColor( Color _color )
{
	if( !wv::Console::isInitialized() )
		wv::Console::initialize();

	if( _color >= Gray )
		_color = (Color)(_color - Gray ); // libcitru does not support bright colors

	if( _color >= Bright_White )
		_color = White;

	printf( wv::Console::Internal::ANSI_EscapeStrsFG[ (int)_color ] );
}

#endif
