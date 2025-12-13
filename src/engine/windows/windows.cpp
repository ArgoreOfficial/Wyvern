#ifdef WV_PLATFORM_WINDOWS
#include <wv/platform/platform.h>

#include <windows/windows_file_system.h>
#include <sdl/display_driver_sdl.h>

#include <wv/memory/memory.h>

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
	WV_FREE( g_displaydriver );
}

#endif