#include "cColor.h"
#include <Wyvern/Math/Math.h>

using namespace wv;

cColor::cColor( byte _r, byte _g, byte _b, byte _a ) :
	m_red( _r ), m_green( _g ), m_blue( _b ), m_alpha( _a )
{

}

cColor::cColor( float _r, float _g, float _b, float _a ) :
	m_red  ( (byte)( wv::Math::clamp( _r, 0.0f, 1.0f ) * 255.0f ) ),
	m_green( (byte)( wv::Math::clamp( _g, 0.0f, 1.0f ) * 255.0f ) ),
	m_blue ( (byte)( wv::Math::clamp( _b, 0.0f, 1.0f ) * 255.0f ) ),
	m_alpha( (byte)( wv::Math::clamp( _a, 0.0f, 1.0f ) * 255.0f ) )
{

}

cColor::cColor( dword _rgba ):
	m_red  ( ( _rgba >> 24 ) & 0xFF ),
	m_green( ( _rgba >> 16 ) & 0xFF ),
	m_blue ( ( _rgba >> 8 ) & 0xFF ),
	m_alpha( _rgba & 0xFF )
{
	
}
