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
class Vector3
{

public:

	_Ty x, y, z;

///////////////////////////////////////////////////////////////////////////////////////

	Vector3( void ) : 
		x( 0 ), 
		y( 0 ), 
		z( 0 ) 
	{ }
	
	Vector3( const _Ty& _t ) : 
		x( _t ), 
		y( _t ),
		z( _t ) 
	{ }

	Vector3( const _Ty& _x, const _Ty& _y, const _Ty& _z ) : 
		x( _x ), 
		y( _y ), 
		z( _z ) 
	{ }

	_Ty length( void ) const {
		return std::sqrt( x * x + y * y + z * z );
	}

	_Ty dot( const Vector3<_Ty>& _other ) const {
		return x * _other.x
			+ y * _other.y
			+ z * _other.z;
	}

	Vector3<_Ty> cross( const Vector3<_Ty>& _other ) const {
		return Vector3<_Ty>(
			y * _other.z - z * _other.y,
			z * _other.x - x * _other.z,
			x * _other.y - y * _other.x
		);
	}

	void normalize( int _magnitude = 1.0f ) {
		_Ty magnitude = length();
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;

		if ( _magnitude != 1.0f )
			*this *= _magnitude;
	}

	Vector3<_Ty> normalized() {
		Vector3 vec = *this;
		vec.normalize();
		return vec;
	}

	static inline Vector3<_Ty> eulerToDirection( Vector3<_Ty> _vec ) {
		_Ty pitch = wv::Math::radians( _vec.x );
		_Ty yaw = wv::Math::radians( _vec.y );

		return vec(
			std::cos( pitch ) * std::sin( yaw ),
			-std::sin( pitch ),
			std::cos( pitch ) * std::cos( yaw )
		);
	}

	static inline Vector3<_Ty> directionToEuler( Vector3<_Ty> _vec ) {
		return Vector3<_Ty>(
			wv::Math::degrees( std::asin( -_vec.y ) ),         // pitch
			wv::Math::degrees( std::atan2( _vec.x, _vec.z ) ), // yaw
			0
		);
	}

	inline Vector3<_Ty> eulerToDirection() { return Vector3<_Ty>::eulerToDirection( *this ); }
	inline Vector3<_Ty> directionToEuler() { return Vector3<_Ty>::directionToEuler( *this ); }

	Vector3<_Ty>& operator = ( const Vector3<_Ty>& _other );
	Vector3<_Ty>& operator +=( const Vector3<_Ty>& _other );
	Vector3<_Ty>& operator -=( const Vector3<_Ty>& _other );
	Vector3<_Ty>  operator + ( const Vector3<_Ty>& _other ) const;
	Vector3<_Ty>  operator - ( const Vector3<_Ty>& _other ) const;
	bool        operator ==( const Vector3<_Ty>& _other ) const;
	bool        operator !=( const Vector3<_Ty>& _other ) const { return !( *this == _other ); }

	Vector3<_Ty>  operator - ( void ) const;
	Vector3<_Ty>  operator * ( const _Ty& _scalar ) const;
	Vector3<_Ty>& operator *=( const _Ty& _scalar );
	Vector3<_Ty>  operator / ( const _Ty& _scalar ) const;
	Vector3<_Ty>& operator /=( const _Ty& _scalar );

#ifdef WV_CPP20
	auto operator<=>( const Vector3<_Ty>& ) const = default;
#endif
};

///////////////////////////////////////////////////////////////////////////////////////

typedef Vector3<float>  Vector3f;
typedef Vector3<double> Vector3d;

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty>
inline Vector3<_Ty>& Vector3<_Ty>::operator=( const Vector3<_Ty>& _other ) {
	x = _other.x;
	y = _other.y;
	z = _other.z;
	return *this;
}

template<typename _Ty>
inline Vector3<_Ty>& Vector3<_Ty>::operator+=( const Vector3<_Ty>& _other )
{
	x += _other.x;
	y += _other.y;
	z += _other.z;
	return *this;
}

template<typename _Ty>
inline Vector3<_Ty>& Vector3<_Ty>::operator-=( const Vector3<_Ty>& _other )
{
	x -= _other.x;
	y -= _other.y;
	z -= _other.z;
	return *this;
}

template<typename _Ty>
inline Vector3<_Ty> Vector3<_Ty>::operator+( const Vector3<_Ty>& _other ) const
{
	return Vector3<_Ty>( x + _other.x, y + _other.y, z + _other.z );
}

template<typename _Ty>
inline Vector3<_Ty> Vector3<_Ty>::operator-( const Vector3<_Ty>& _other ) const
{
	return Vector3<_Ty>( x - _other.x, y - _other.y, z - _other.z );
}

template<typename _Ty>
inline bool Vector3<_Ty>::operator==( const Vector3<_Ty>& _other ) const
{
	return x == _other.x
		&& y == _other.y
		&& z == _other.z;
}

template<typename _Ty>
inline Vector3<_Ty> Vector3<_Ty>::operator-( void ) const
{
	return Vector3<_Ty>( -x, -y, -z );
}

template<typename _Ty>
inline Vector3<_Ty> wv::Vector3<_Ty>::operator*( const _Ty& _scalar ) const
{
	return Vector3<_Ty>( x * _scalar, y * _scalar, z * _scalar );
}

template<typename _Ty>
inline Vector3<_Ty>& wv::Vector3<_Ty>::operator*=( const _Ty& _scalar )
{
	x *= _scalar;
	y *= _scalar;
	z *= _scalar;
	return *this;
}

template<typename _Ty>
inline Vector3<_Ty> wv::Vector3<_Ty>::operator/( const _Ty& _scalar ) const
{
	return Vector3<_Ty>( x / _scalar, y / _scalar, z / _scalar );
}

template<typename _Ty>
inline Vector3<_Ty>& wv::Vector3<_Ty>::operator/=( const _Ty& _scalar )
{
	x /= _scalar;
	y /= _scalar;
	z /= _scalar;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

}