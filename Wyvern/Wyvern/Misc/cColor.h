#pragma once
#include <Wyvern/Core/defs.h>

namespace wv
{
	namespace Color
	{
		enum
		{
			White = 0xFFFFFFFF,
			Black = 0x000000Ff,
			Red = 0xFF0000FF,
			Blue = 0x00FF00FF,
			PacificBlue = 0x039BCFFF,
			Green = 0x0000FFFF
		};
	}

	class cColor
	{
	public:

		cColor( byte _r, byte _g, byte _b, byte _a = 255 );
		cColor( float _r, float _g, float _b, float _a = 1.0f );
		cColor( dword _rgba );

		byte m_red = 0x00, m_blue = 0x00, m_green = 0x00, m_alpha = 0xFF;

	};
}