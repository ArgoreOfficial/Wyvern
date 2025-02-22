#pragma once

#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <stdint.h>

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
		
		constexpr Color( byte _r, byte _g, byte _b, byte _a = 255  );
		Color( wv::Vector3< byte > _rgb, byte _a = 255  );
		Color( wv::Vector4< byte > _rgba );
		Color( wv::Vector3< float > _rgb, float _a = 1.0f );
		Color( wv::Vector4< float > _rgba );
		constexpr Color( float _r, float _g, float _b, float _a = 1.0f );
		constexpr Color( dword _rgba );
		

///////////////////////////////////////////////////////////////////////////////////////

		byte r = 0x00;
		byte g = 0x00;
		byte b = 0x00;
		byte a = 0xFF;
		
		
		bool operator == ( Color& _other );
		byte& operator[]( const size_t& _index );

		static constexpr dword White       = 0xFFFFFFFF;
		static constexpr dword Gray        = 0x0F0F0FFF;
		static constexpr dword Black       = 0x000000FF;
		static constexpr dword Red         = 0xFF0000FF;
		static constexpr dword Yellow      = 0xFFFF00FF;
		static constexpr dword Turquoise   = 0x30D5C8FF;
		static constexpr dword Cyan        = 0x00FFFFFF;
		static constexpr dword Blue        = 0x00FF00FF;
		static constexpr dword Magenta     = 0xFF00FFFF;
		static constexpr dword PacificBlue = 0x039BCFFF;
		static constexpr dword Green       = 0x0000FFFF;
	};

///////////////////////////////////////////////////////////////////////////////////////

}