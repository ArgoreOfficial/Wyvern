#pragma once

#include <wv/helpers/strong_type.hpp>
#include <wv/resource_id.h>

#include <stdint.h>
#include <stdexcept>
#include <string>
#include <memory>

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

typedef unsigned int GLHandle; // same as GLuint

// WV_DEFINE_ID( ResourceID );
WV_DEFINE_ID( ObjectID );

template<typename Ty>
using Ref = std::shared_ptr<Ty>;

template<typename Ty>
using WeakRef = std::weak_ptr<Ty>;

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