#include "Color.h"

#include <wv/math/Math.h>

///////////////////////////////////////////////////////////////////////////////////////

constexpr wv::Color::Color( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) :
	r{ _r }, 
	g{ _g }, 
	b{ _b }, 
	a{ _a }
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Color::Color( wv::Vector3<uint8_t> _rgb, uint8_t _a ) :
	r{ _rgb.x }, 
	g{ _rgb.y }, 
	b{ _rgb.z }, 
	a{ _a }
{
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Color::Color( wv::Vector4<uint8_t> _rgba ) :
	r{ _rgba.x }, 
	g{ _rgba.y }, 
	b{ _rgba.z }, 
	a{ _rgba.w }
{
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Color::Color( wv::Vector3<float> _rgb, float _a ) :
	r{ static_cast<uint8_t>( wv::Math::clamp( _rgb.x, 0.0f, 1.0f ) * 255.0f ) }, 
	g{ static_cast<uint8_t>( wv::Math::clamp( _rgb.y, 0.0f, 1.0f ) * 255.0f ) }, 
	b{ static_cast<uint8_t>( wv::Math::clamp( _rgb.z, 0.0f, 1.0f ) * 255.0f ) }, 
	a{ static_cast<uint8_t>( wv::Math::clamp( _a, 0.0f, 1.0f ) * 255.0f ) }
{
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Color::Color( wv::Vector4<float> _rgba ) :
	r{ static_cast<uint8_t>( wv::Math::clamp( _rgba.x, 0.0f, 1.0f ) * 255.0f ) }, 
	g{ static_cast<uint8_t>( wv::Math::clamp( _rgba.y, 0.0f, 1.0f ) * 255.0f ) }, 
	b{ static_cast<uint8_t>( wv::Math::clamp( _rgba.z, 0.0f, 1.0f ) * 255.0f ) }, 
	a{ static_cast<uint8_t>( wv::Math::clamp( _rgba.w, 0.0f, 1.0f ) * 255.0f ) }
{
}

///////////////////////////////////////////////////////////////////////////////////////

constexpr wv::Color::Color( float _r, float _g, float _b, float _a ) :
	r{ static_cast<uint8_t>( wv::Math::clamp( _r, 0.0f, 1.0f ) * 255.0f ) },
	g{ static_cast<uint8_t>( wv::Math::clamp( _g, 0.0f, 1.0f ) * 255.0f ) },
	b{ static_cast<uint8_t>( wv::Math::clamp( _b, 0.0f, 1.0f ) * 255.0f ) },
	a{ static_cast<uint8_t>( wv::Math::clamp( _a, 0.0f, 1.0f ) * 255.0f ) }
{

}

///////////////////////////////////////////////////////////////////////////////////////

constexpr wv::Color::Color( uint32_t _rgba ):
	r{ ( _rgba >> 24 ) & 0xFF },
	g{ ( _rgba >> 16 ) & 0xFF },
	b{ ( _rgba >> 8  ) & 0xFF },
	a{   _rgba         & 0xFF }
{
	
}
///////////////////////////////////////////////////////////////////////////////////////

bool wv::Color::operator==( Color& _other )
{
	return r == _other.r && g == _other.g && b == _other.b && a == _other.a;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::uint8_t& wv::Color::operator[]( const size_t& _index )
{
	return ( &r )[ _index ];
}

