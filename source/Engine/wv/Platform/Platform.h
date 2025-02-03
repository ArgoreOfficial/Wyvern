#pragma once

#if defined( WV_PLATFORM_WINDOWS )
#include <wv/Platform/Windows/Windows.h>
#elif defined( WV_PLATFORM_PSVITA )
#include <wv/Platform/PSVita/PSVita.h>
#elif defined( WV_PLATFORM_3DS )
#include <wv/Platform/3DS/3DS.h>
#endif