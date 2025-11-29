#pragma once

#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Color
	{
		
	public:
		
		constexpr Color( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255  );
		Color( wv::Vector3< uint8_t > _rgb, byte _a = 255  );
		Color( wv::Vector4< uint8_t > _rgba );
		Color( wv::Vector3< float > _rgb, float _a = 1.0f );
		Color( wv::Vector4< float > _rgba );
		constexpr Color( float _r, float _g, float _b, float _a = 1.0f );
		constexpr Color( uint32_t _rgba );
		

///////////////////////////////////////////////////////////////////////////////////////

		uint8_t r = 0x00;
		uint8_t g = 0x00;
		uint8_t b = 0x00;
		uint8_t a = 0xFF;
		
		bool     operator==( const Color& _other );
		uint8_t& operator[]( const size_t& _index );

		static constexpr uint32_t White       = 0xFFFFFFFF;
		static constexpr uint32_t Gray        = 0x0F0F0FFF;
		static constexpr uint32_t Black       = 0x000000FF;
		static constexpr uint32_t Red         = 0xFF0000FF;
		static constexpr uint32_t Yellow      = 0xFFFF00FF;
		static constexpr uint32_t Turquoise   = 0x30D5C8FF;
		static constexpr uint32_t Cyan        = 0x00FFFFFF;
		static constexpr uint32_t Blue        = 0x00FF00FF;
		static constexpr uint32_t Magenta     = 0xFF00FFFF;
		static constexpr uint32_t PacificBlue = 0x039BCFFF;
		static constexpr uint32_t Green       = 0x0000FFFF;
	};

///////////////////////////////////////////////////////////////////////////////////////

}