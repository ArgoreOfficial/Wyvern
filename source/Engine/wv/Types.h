#pragma once

#include <stdint.h>
#include <stdexcept>
#include <string>

#include <wv/strong_type.hpp>

///////////////////////////////////////////////////////////////////////////////////////

// psvita compatability
#ifdef WV_PLATFORM_PSVITA
typedef unsigned int size_t;
#endif

#define WV_DEFINE_ID( _c ) typedef wv::strong_type<uint16_t, struct _c##_t> _c

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

typedef void* ( *GraphicsDriverLoadProc )( const char* _name );

typedef uint32_t Handle;
typedef uint64_t UUID;

///////////////////////////////////////////////////////////////////////////////////////

enum DataType
{
	WV_BYTE,
	WV_UNSIGNED_BYTE,
	WV_SHORT,
	WV_UNSIGNED_SHORT,
	WV_INT,
	WV_UNSIGNED_INT,
	WV_FLOAT,
#ifndef EMSCRIPTEN
	WV_DOUBLE
#endif
};

///////////////////////////////////////////////////////////////////////////////////////

enum GraphicsAPI
{
	WV_GRAPHICS_API_NONE

#ifdef WV_SUPPORT_OPENGL
	, WV_GRAPHICS_API_OPENGL
	, WV_GRAPHICS_API_OPENGL_ES1
	, WV_GRAPHICS_API_OPENGL_ES2
#endif

#ifdef WV_PLATFORM_PSVITA
	, WV_GRAPHICS_API_PSVITA
#endif

#ifdef WV_PLATFORM_3DS
	, WV_GRAPHICS_API_CITRA
#endif
};

///////////////////////////////////////////////////////////////////////////////////////

union GenericVersion
{
	struct
	{
		unsigned short major;
		unsigned short minor;
	};
	int i;

	inline bool isAtleast( unsigned short _major, unsigned short _minor ) { return ( major >= _major ) && ( major > _major || minor >= _minor ); }

};

///////////////////////////////////////////////////////////////////////////////////////

}

