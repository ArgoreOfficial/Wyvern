#pragma once

#include <cmath>
#include <wv/math/math.h>

#ifdef WV_CPP20
#include <compare>
#endif

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
class Vector3
{

public:

	Ty x, y, z;

///////////////////////////////////////////////////////////////////////////////////////

	Vector3( void ) : 
		x( 0 ), 
		y( 0 ), 
		z( 0 ) 
	{ }
	
	Vector3( const Ty& _t ) : 
		x( _t ), 
		y( _t ),
		z( _t ) 
	{ }

	Vector3( const Ty& _x, const Ty& _y, const Ty& _z ) : 
		x( _x ), 
		y( _y ), 
		z( _z ) 
	{ }

	Ty length( void ) const {
		return std::sqrt( x * x + y * y + z * z );
	}

	Ty dot( const Vector3<Ty>& _other ) const {
		return x * _other.x
			 + y * _other.y
			 + z * _other.z;
	}

	Vector3<Ty> cross( const Vector3<Ty>& _other ) const {
		return Vector3<Ty>(
			y * _other.z - z * _other.y,
			z * _other.x - x * _other.z,
			x * _other.y - y * _other.x
		);
	}

	void normalize( Ty _magnitude = 1.0 ) {
		Ty magnitude = length();
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;

		if ( _magnitude != 1.0f )
			*this *= _magnitude;
	}

	Vector3<Ty> normalized( Ty _magnitude = 1.0 ) const {
		Vector3 vec{ x, y, z };
		vec.normalize( _magnitude );
		return vec;
	}

	static inline Vector3<Ty> up() {
		return { 0.0f, 1.0f, 0.0 };
	}

	static inline Vector3<Ty> eulerToDirection( Vector3<Ty> _vec ) {
		Ty pitch = wv::Math::radians( _vec.x );
		Ty yaw = wv::Math::radians( _vec.y );

		return Vector3<Ty>(
			 std::cos( pitch ) * std::sin( yaw ),
			-std::sin( pitch ),
			 std::cos( pitch ) * std::cos( yaw )
		);
	}

	static inline Vector3<Ty> directionToEuler( Vector3<Ty> _vec ) {
		return Vector3<Ty>(
			wv::Math::degrees( std::asin( -_vec.y ) ),         // pitch
			wv::Math::degrees( std::atan2( _vec.x, _vec.z ) ), // yaw
			0
		);
	}

	inline Vector3<Ty> eulerToDirection() { return Vector3<Ty>::eulerToDirection( *this ); }
	inline Vector3<Ty> directionToEuler() { return Vector3<Ty>::directionToEuler( *this ); }

	Vector3<Ty>& operator = ( const Vector3<Ty>& _other );
	Vector3<Ty>& operator +=( const Vector3<Ty>& _other );
	Vector3<Ty>& operator -=( const Vector3<Ty>& _other );
	Vector3<Ty>  operator + ( const Vector3<Ty>& _other ) const;
	Vector3<Ty>  operator - ( const Vector3<Ty>& _other ) const;
	bool        operator ==( const Vector3<Ty>& _other ) const;
	bool        operator !=( const Vector3<Ty>& _other ) const { return !( *this == _other ); }

	Vector3<Ty>  operator - ( void ) const;
	Vector3<Ty>  operator * ( const Ty& _scalar ) const;
	Vector3<Ty>& operator *=( const Ty& _scalar );
	Vector3<Ty>  operator / ( const Ty& _scalar ) const;
	Vector3<Ty>& operator /=( const Ty& _scalar );

#ifdef WV_CPP20
	auto operator<=>( const Vector3<Ty>& ) const = default;
#endif
};

///////////////////////////////////////////////////////////////////////////////////////

typedef Vector3<float>  Vector3f;
typedef Vector3<double> Vector3d;

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
inline Vector3<Ty>& Vector3<Ty>::operator=( const Vector3<Ty>& _other ) {
	x = _other.x;
	y = _other.y;
	z = _other.z;
	return *this;
}

template<typename Ty>
inline Vector3<Ty>& Vector3<Ty>::operator+=( const Vector3<Ty>& _other )
{
	x += _other.x;
	y += _other.y;
	z += _other.z;
	return *this;
}

template<typename Ty>
inline Vector3<Ty>& Vector3<Ty>::operator-=( const Vector3<Ty>& _other )
{
	x -= _other.x;
	y -= _other.y;
	z -= _other.z;
	return *this;
}

template<typename Ty>
inline Vector3<Ty> Vector3<Ty>::operator+( const Vector3<Ty>& _other ) const
{
	return Vector3<Ty>( x + _other.x, y + _other.y, z + _other.z );
}

template<typename Ty>
inline Vector3<Ty> Vector3<Ty>::operator-( const Vector3<Ty>& _other ) const
{
	return Vector3<Ty>( x - _other.x, y - _other.y, z - _other.z );
}

template<typename Ty>
inline bool Vector3<Ty>::operator==( const Vector3<Ty>& _other ) const
{
	return x == _other.x
		&& y == _other.y
		&& z == _other.z;
}

template<typename Ty>
inline Vector3<Ty> Vector3<Ty>::operator-( void ) const
{
	return Vector3<Ty>( -x, -y, -z );
}

template<typename Ty>
inline Vector3<Ty> wv::Vector3<Ty>::operator*( const Ty& _scalar ) const
{
	return Vector3<Ty>( x * _scalar, y * _scalar, z * _scalar );
}

template<typename Ty>
inline Vector3<Ty>& wv::Vector3<Ty>::operator*=( const Ty& _scalar )
{
	x *= _scalar;
	y *= _scalar;
	z *= _scalar;
	return *this;
}

template<typename Ty>
inline Vector3<Ty> wv::Vector3<Ty>::operator/( const Ty& _scalar ) const
{
	return Vector3<Ty>( x / _scalar, y / _scalar, z / _scalar );
}

template<typename Ty>
inline Vector3<Ty>& wv::Vector3<Ty>::operator/=( const Ty& _scalar )
{
	x /= _scalar;
	y /= _scalar;
	z /= _scalar;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

namespace Math {

template<typename Ty>
Ty angleBetween( const Vector3<Ty>& _a, const Vector3<Ty>& _b )
{
	return std::atan2( _a.cross( _b ).length(), _a.dot( _b ) );
}

template<typename Ty>
Vector3<Ty> slerp( const Vector3<Ty>& _a, const Vector3<Ty>& _b, double _t )
{
	Vector3<Ty> a = _a.normalized();
	Vector3<Ty> b = _b.normalized();

	// angle between normalized vector a and b
	double phi = angleBetween( a, b );

	const Ty tA = (Ty)std::sin( ( 1 - _t ) * phi ) / std::sin( phi );
	const Ty tB = (Ty)std::sin(         _t * phi ) / std::sin( phi );

	return ( _a * tA ) + ( _b * tB );
}

}

}