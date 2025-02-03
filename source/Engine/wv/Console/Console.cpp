#include "Console.h"

#ifdef WV_PLATFORM_3DS
#include <wv/Platform/3DS/3DS.h>

#include <3ds.h>
#include <citro3d.h>
#endif

bool wv::Console::Internal::g_initialized = false;

bool wv::Console::isInitialized()
{
    return wv::Console::Internal::g_initialized;
}

void wv::Console::initialize()
{

#ifdef WV_PLATFORM_3DS
	/// TOOD: move to platform specific
	// platform init
	gfxInitDefault();
	C3D_Init( C3D_DEFAULT_CMDBUF_SIZE );
	// console init
	consoleInit( GFX_BOTTOM, 0 );
#endif

    wv::Console::Internal::g_initialized = true;
}
