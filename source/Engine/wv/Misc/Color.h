#pragma once

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	/// TODO: not this
	typedef unsigned char byte;
	typedef unsigned int dword;

	class Color
	{

	public:

		Color( byte _r, byte _g, byte _b, byte _a = 255  );
		Color( wv::Vector3< byte > _rgb, byte _a = 255  );
		Color( wv::Vector4< byte > _rgba );
		Color( wv::Vector3< float > _rgb, float _a = 1.0f );
		Color( wv::Vector4< float > _rgba );
		Color( float _r, float _g, float _b, float _a = 1.0f );
		Color( dword _rgba );

///////////////////////////////////////////////////////////////////////////////////////

		byte r = 0x00;
		byte g = 0x00;
		byte b = 0x00;
		byte a = 0xFF;

		bool operator == ( Color& _other );
		byte& operator[]( const size_t& _index );

	};

///////////////////////////////////////////////////////////////////////////////////////

	namespace Colors
	{
		const Color White      ( 0xFFFFFFFF );
		const Color Gray       ( 0x0F0F0FFF );
		const Color Black      ( 0x000000FF );
		const Color Red        ( 0xFF0000FF );
		const Color Yellow     ( 0xFFFF00FF );
		const Color Turquoise  ( 0x30D5C8FF );
		const Color Cyan       ( 0x00FFFFFF );
		const Color Blue       ( 0x00FF00FF );
		const Color Magenta    ( 0xFF00FFFF );
		const Color PacificBlue( 0x039BCFFF );
		const Color Green      ( 0x0000FFFF );
	}

///////////////////////////////////////////////////////////////////////////////////////

}