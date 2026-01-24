#ifdef WV_PLATFORM_WINDOWS
#include <wv/platform/platform.h>

#include <windows/windows_file_system.h>
#include <sdl/display_driver_sdl.h>

#include <wv/memory/memory.h>
#include <wv/debug/log.h>

#include "windows.h"

static wv::WindowsFileSystem* g_filesystem = nullptr;
static wv::DisplayDriverSDL*  g_displaydriver = nullptr;

bool wv::Platform::initialize()
{
	// nothing special to initialize
	return true;
}

wv::IFileSystem* wv::Platform::createFileSystem( const std::string& _mountedName ) {
	g_filesystem = WV_NEW( WindowsFileSystem );
	g_filesystem->mount( _mountedName );
	return g_filesystem;
}

wv::DisplayDriver* wv::Platform::createDisplayDriver() {
	g_displaydriver = WV_NEW( DisplayDriverSDL );
	return g_displaydriver;
}

void wv::Platform::cleanup() {
	WV_FREE( g_filesystem );
}

#endif

void wv::Windows::printLastError()
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	if ( FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf,
		0, NULL ) == 0 )
	{
		WV_LOG_ERROR( "WINDOWS ERROR: UNKNOWN - FAILED TO FORMAT\n" );
	}

	WV_LOG_ERROR( "WINDOWS ERROR: %s\n", (LPCTSTR)lpMsgBuf );

	LocalFree( lpMsgBuf );
}