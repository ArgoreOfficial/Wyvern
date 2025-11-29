#ifdef WV_PLATFORM_WINDOWS

#include "windows_console.h"

#include <stdio.h>
#include <Windows.h>

static HANDLE hConsole = nullptr;
// https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/color
static WORD g_colorAttribs[] = {
	0x0, // Black
	0x4, // Red
	0x2, // Green
	0x6, // Yellow
	0x1, // Blue
	0x5, // Purple
	0x3, // Aqua
	0x7, // White
	0x8, // Gray
	0xc, // Light red
	0xa, // Light green
	0xe, // Light yellow
	0x9, // Light blue
	0xd, // Light purple
	0xb, // Light aqua
	0xf  // Bright white
};

void wv::Console::initialize()
{
	hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

	wv::Console::Internal::g_initialized = true;
}

void wv::Console::deinitialize()
{
	wv::Console::Internal::g_initialized = false;
}

void wv::Console::setForegroundColor( Color _color )
{
	if( !wv::Console::isInitialized() )
		wv::Console::initialize();

	WORD colorAttrib = g_colorAttribs[ _color ];

	SetConsoleTextAttribute( hConsole, colorAttrib );
}

#endif