#pragma once

#include <stdint.h>

namespace wv
{
	typedef void* ( *GraphicsDriverLoadProc )( const char* _name );
	typedef uint32_t Handle;

	enum DataType
	{
		WV_BYTE,
		WV_UNSIGNED_BYTE,
		WV_SHORT,
		WV_UNSIGNED_SHORT,
		WV_INT,
		WV_UNSIGNED_INT,
		WV_FLOAT,
		WV_DOUBLE
	};

	enum GraphicsAPI
	{
		WV_GRAPHICS_API_OPENGL
	};

	struct GenericVersion
	{
		int minor;
		int major;
	};
}