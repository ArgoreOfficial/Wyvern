#pragma once

namespace wv
{
	typedef void* ( *GraphicsDriverLoadProc )( const char* _name );

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