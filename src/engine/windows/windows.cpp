#ifdef WV_PLATFORM_WINDOWS
#include <wv/platform/platform.h>

#include <windows/windows_file_system.h>

wv::IFileSystem* wv::Platform::createFileSystem( const std::string& _mountedName ) {
	wv::WindowsFileSystem* fs = new WindowsFileSystem();
	fs->mount( _mountedName );
	return fs;
}

#endif