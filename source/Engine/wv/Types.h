#pragma once

#include <stdint.h>

#include <wv/Types/StrongID.h>
#include <stdexcept>

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

	/// move to graphics types.h (after graphics -> lowlevelgraphics rework)
	WV_DEFINE_ID( PipelineID );
	WV_DEFINE_ID( RenderTargetID );
	WV_DEFINE_ID( ShaderProgramID );

///////////////////////////////////////////////////////////////////////////////////////

}

