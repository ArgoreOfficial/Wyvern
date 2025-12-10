#ifdef WV_PLATFORM_WINDOWS
#include <wv/platform/platform.h>

#include <windows/windows_file_system.h>

#ifdef WV_PACKAGE
const char* wv::gFileSystemPathPrefix = "./";
#else
const char* wv::gFileSystemPathPrefix = "../../";
#endif

wv::IFileSystem* wv::Platform::createFileSystem() {
	return new WindowsFileSystem();
}

#endif