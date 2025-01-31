#pragma once

#include <stdint.h>
#include <stdexcept>
#include <string>

#include <wv/Types/StrongID.h>

///////////////////////////////////////////////////////////////////////////////////////

// psvita compatability
#ifdef WV_PLATFORM_PSVITA
typedef unsigned int size_t;
#endif

// funny
#define WV_SLASH_HELPER( _f ) /_f
#define WV_COMMENT WV_SLASH_HELPER(/)

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
		,WV_GRAPHICS_API_OPENGL
		,WV_GRAPHICS_API_OPENGL_ES1
		,WV_GRAPHICS_API_OPENGL_ES2
	#endif

	#ifdef WV_PLATFORM_PSVITA
		,WV_GRAPHICS_API_PSVITA
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

	template<typename T> static const char* typeStr() { return "unknown type"; }

#define WV_DEFINE_TYPE_STR( _type ) template<> const char* typeStr<_type>() { return #_type; }

	WV_DEFINE_TYPE_STR( char )
	WV_DEFINE_TYPE_STR( unsigned char )
	WV_DEFINE_TYPE_STR( int )
	WV_DEFINE_TYPE_STR( unsigned int )
	WV_DEFINE_TYPE_STR( float )
	WV_DEFINE_TYPE_STR( double )
	WV_DEFINE_TYPE_STR( bool )
	WV_DEFINE_TYPE_STR( std::string )
	WV_DEFINE_TYPE_STR( const char* )

///////////////////////////////////////////////////////////////////////////////////////

}

