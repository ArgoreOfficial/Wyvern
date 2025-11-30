#ifdef WV_PLATFORM_WINDOWS
#include <wv/platform/platform.h>

#ifdef WV_PACKAGE
const char* wv::gFileSystemPathPrefix = "";
#else
const char* wv::gFileSystemPathPrefix = "../../";
#endif
#endif