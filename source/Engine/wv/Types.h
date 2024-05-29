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
	#ifndef EMSCRIPTEN
		WV_DOUBLE
	#endif
	};

	enum GraphicsAPI
	{
		WV_GRAPHICS_API_OPENGL,
		WV_GRAPHICS_API_OPENGL_ES1,
		WV_GRAPHICS_API_OPENGL_ES2
	};

	struct GenericVersion
	{
		int minor;
		int major;
	};

	/// TEMPORARY
	/// TODO: get wv::Vector3<T>
	struct float3
	{
		float x, y, z;
	};
}