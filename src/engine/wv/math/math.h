#pragma once

#include <wv/types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
namespace Const /// TODO: move
{
namespace Float
{
constexpr float PI = 3.141592f;
constexpr float EPSILON = 0.00000001f;
}

namespace Double
{
constexpr float PI = 3.141592;
constexpr float EPSILON = 0.000001;
}
}

///////////////////////////////////////////////////////////////////////////////////////

namespace Math
{

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty> constexpr _Ty clamp( const _Ty& _value, const _Ty& _min, const _Ty& _max );

template<typename _Ty> _Ty pow2( const _Ty& _value );
template<typename _Ty> _Ty pow3( const _Ty& _value );
template<typename _Ty> _Ty pow4( const _Ty& _value );
template<typename _Ty> _Ty pow5( const _Ty& _value );

template<typename _Ty> _Ty abs( const _Ty& _value );

template<typename _Ty> _Ty min( const _Ty& _a, const _Ty& _b );
template<typename _Ty> _Ty max( const _Ty& _a, const _Ty& _b );

template<typename _Ty> _Ty lerp( const _Ty& _a, const _Ty& _b, const _Ty& _t );

inline float  radians( const float& _degrees ) { 
	return _degrees * ( Const::Float::PI / 180.0f ); 
}

inline double radians( const double& _degrees ) { 
	return _degrees * ( Const::Double::PI / 180.0 ); 
}

inline float  degrees( const float& _radians )  { 
	return _radians * ( 180.0f / Const::Float::PI ); 
}

inline double degrees( const double& _radians ) { 
	return _radians * ( 180.0 / Const::Double::PI ); 
}

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty> constexpr _Ty clamp( const _Ty& _value, const _Ty& _min, const _Ty& _max )
{
	const _Ty t = _value < _min ? _min : _value;
	return t > _max ? _max : t;
}

template<typename _Ty> _Ty pow2( const _Ty& _value )
{
	return _value * _value;
}

template<typename _Ty> _Ty pow3( const _Ty& _value )
{
	return _value * _value * _value;
}

template<typename _Ty> _Ty pow4( const _Ty& _value )
{
	return _value * _value * _value * _value;
}

template<typename _Ty> _Ty pow5( const _Ty& _value )
{
	return _value * _value * _value * _value * _value;
}

template<typename _Ty> _Ty abs( const _Ty& _value )
{
	return _value < _Ty( 0 ) ? -_value : _value;
}

template<typename _Ty> _Ty min( const _Ty& _a, const _Ty& _b )
{
	return _a < _b ? _a : _b;
}

template<typename _Ty> _Ty max( const _Ty& _a, const _Ty& _b )
{
	return _a > _b ? _a : _b;
}

template<typename _Ty>
_Ty lerp( const _Ty& _a, const _Ty& _b, const _Ty& _t )
{
	return _a + _t * ( _b - _a );
}

///////////////////////////////////////////////////////////////////////////////////////

}

}