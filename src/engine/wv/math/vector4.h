#pragma once

#include <cmath>
#include <wv/math/Math.h>

#ifdef WV_CPP20
#include <compare>
#endif

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty>
class Vector4
{

public:

	_Ty x, y, z, w;

///////////////////////////////////////////////////////////////////////////////////////

	Vector4( void ) : 
		x{ 0 }, 
		y{ 0 }, 
		z{ 0 }, 
		w{ 0 } 
	{ }
	
	Vector4( const _Ty& _t ) : 
		x{ _t }, 
		y{ _t }, 
		z{ _t }, 
		w{ _t } 
	{ }
	
	Vector4( const _Ty& _x, const _Ty& _y, const _Ty& _z, const _Ty& _w ) : 
		x{ _x }, 
		y{ _y }, 
		z{ _z }, 
		w{ _w } 
	{ }

	_Ty length( void )const {
		return std::sqrt( x * x + y * y + z * z + w * w );
	}

	_Ty dot( const Vector4<_Ty>& _other ) const {
		return x * _other.x
			+ y * _other.y
			+ z * _other.z
			+ w * _other.w;
	}

	void normalize( int _magnitude = 1.0f ) {
		_Ty magnitude = length();
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		w /= magnitude;

		if ( _magnitude != 1.0f )
			*this *= _magnitude;
	}

	Vector4<_Ty> normalized() const {
		Vector4<_Ty> vec = *this;
		vec.normalize();
		return vec;
	}

	Vector4<_Ty>& operator = ( const Vector4<_Ty>& _other );
	Vector4<_Ty>& operator +=( const Vector4<_Ty>& _other );
	Vector4<_Ty>  operator + ( const Vector4<_Ty>& _other );
	Vector4<_Ty>  operator * ( const float& _scalar );
	Vector4<_Ty>& operator *=( const float& _scalar );
	Vector4<_Ty>  operator / ( const float& _scalar );
	Vector4<_Ty>& operator /=( const float& _scalar );

#ifdef WV_CPP20
	auto operator<=>( const Vector4<_Ty>& ) const = default;
#endif

};

///////////////////////////////////////////////////////////////////////////////////////

typedef Vector4<float>  Vector4f;
typedef Vector4<double> Vector4d;

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty>
inline Vector4<_Ty>& Vector4<_Ty>::operator=( const Vector4<_Ty>& _other )
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;
	return *this;
}

template<typename _Ty>
inline Vector4<_Ty>& Vector4<_Ty>::operator+=( const Vector4<_Ty>& _other )
{
	x += _other.x;
	y += _other.y;
	z += _other.z;
	w += _other.w;
	return *this;
}

template<typename _Ty>
inline Vector4<_Ty> Vector4<_Ty>::operator+( const Vector4<_Ty>& _other )
{
	return Vector4<_Ty>( x + _other.x, y + _other.y, z + _other.z, w + _other.w );
}

template<typename _Ty>
inline Vector4<_Ty> wv::Vector4<_Ty>::operator*( const float& _scalar )
{
	return Vector4<_Ty>( x * _scalar, y * _scalar, z * _scalar, w * _scalar );
}

template<typename _Ty>
inline Vector4<_Ty>& Vector4<_Ty>::operator*=( const float& _scalar )
{
	x *= _scalar;
	y *= _scalar;
	z *= _scalar;
	w *= _scalar;
	return *this;
}

template<typename _Ty>
inline Vector4<_Ty> wv::Vector4<_Ty>::operator/( const float& _scalar )
{
	return Vector4<_Ty>( x / _scalar, y / _scalar, z / _scalar, w / _scalar );
}

template<typename _Ty>
inline Vector4<_Ty>& Vector4<_Ty>::operator/=( const float& _scalar )
{
	x /= _scalar;
	y /= _scalar;
	z /= _scalar;
	w /= _scalar;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

}