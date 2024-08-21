#pragma once

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////

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
		WV_GRAPHICS_API_OPENGL,
		WV_GRAPHICS_API_OPENGL_ES1,
		WV_GRAPHICS_API_OPENGL_ES2
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

}