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
class Vector2
{

public:

	Ty x, y;

///////////////////////////////////////////////////////////////////////////////////////

	Vector2( void ) : 
		x( 0 ), 
		y( 0 ) 
	{ }
	
	Vector2( const Ty& _t ) : 
		x( _t ), 
		y( _t ) 
	{ }
	
	Vector2( const Ty& _x, const Ty& _y ) : 
		x( _x ), 
		y( _y ) 
	{ }

	template<typename Ty2>
	Vector2( const Vector2<Ty2>& _other ) :
		x( static_cast<Ty>( _other.x ) ),
		y( static_cast<Ty>( _other.y ) )
	{ }

	Ty length() const;
	Ty dot( const Vector2<Ty>& _other ) const;

	void normalize( Ty _magnitude = 1.0 );
	Vector2<Ty> normalized() const;

	bool isZero() const { return ( x == (Ty)0 && y == (Ty)0 ); }

	Vector2<Ty>& operator = ( const Vector2<Ty>& _other );
	Vector2<Ty>& operator +=( const Vector2<Ty>& _other );
	Vector2<Ty>& operator -=( const Vector2<Ty>& _other );
	Vector2<Ty>  operator + ( const Vector2<Ty>& _other ) const;
	Vector2<Ty>  operator - ( const Vector2<Ty>& _other ) const;
	Vector2<Ty>  operator * ( const float& _scalar ) const;
	Vector2<Ty>& operator *=( const float& _scalar );
	Vector2<Ty>  operator / ( const float& _scalar ) const;
	Vector2<Ty>& operator /=( const float& _scalar );

#ifdef WV_CPP20
	auto operator<=>( const Vector2<Ty>& ) const = default;
#endif

	
};

///////////////////////////////////////////////////////////////////////////////////////

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;
typedef Vector2<int> Vector2i;

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
inline Ty Vector2<Ty>::length() const
{
	return std::sqrt( x * x + y * y );
}

template<typename Ty>
inline Ty Vector2<Ty>::dot( const Vector2<Ty>& _other ) const
{
	return
		x * _other.x +
		y * _other.y;
}

template<typename Ty>
void Vector2<Ty>::normalize( Ty _magnitude )
{
	Ty magnitude = length();
	x /= magnitude;
	y /= magnitude;

	if ( _magnitude != 1.0f )
		*this *= _magnitude;
}

template<typename Ty>
Vector2<Ty> Vector2<Ty>::normalized() const
{
	Vector2 vec{ x, y };
	vec.normalize();
	return vec;
}

template<typename Ty>
inline Vector2<Ty>& Vector2<Ty>::operator=( const Vector2<Ty>& _other )
{
	x = _other.x;
	y = _other.y;
	return *this;
}

template<typename Ty>
inline Vector2<Ty>& Vector2<Ty>::operator+=( const Vector2<Ty>& _other )
{
	x += _other.x;
	y += _other.y;
	return *this;
}

template<typename Ty>
inline Vector2<Ty>& Vector2<Ty>::operator-=( const Vector2<Ty>& _other )
{
	x -= _other.x;
	y -= _other.y;
	return *this;
}

template<typename Ty>
inline Vector2<Ty> Vector2<Ty>::operator+( const Vector2<Ty>& _other ) const
{
	return Vector2<Ty>( x + _other.x, y + _other.y );
}

template<typename Ty>
inline Vector2<Ty> Vector2<Ty>::operator-( const Vector2<Ty>& _other ) const
{
	return Vector2<Ty>( x - _other.x, y - _other.y );
}

template<typename Ty>
inline Vector2<Ty> wv::Vector2<Ty>::operator*( const float& _scalar ) const
{
	return Vector2<Ty>( x * _scalar, y * _scalar );
}

template<typename Ty>
inline Vector2<Ty>& Vector2<Ty>::operator*=( const float& _scalar )
{
	x *= _scalar;
	y *= _scalar;
	return *this;
}

template<typename Ty>
inline Vector2<Ty> wv::Vector2<Ty>::operator/( const float& _scalar ) const
{
	return Vector2<Ty>( x / _scalar, y / _scalar );
}

template<typename Ty>
inline Vector2<Ty>& Vector2<Ty>::operator/=( const float& _scalar )
{
	x /= _scalar;
	y /= _scalar;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

namespace Math {

template<typename Ty>
Ty angleBetween( const Vector2<Ty>& _a, const Vector2<Ty>& _b, bool _positive = false )
{
	Ty det = ( _a.x * _b.y ) - ( _a.y * _b.x );
	Ty angle = -std::atan2( det, _a.dot( _b ) );
	
	if ( _positive && angle < 0 )
		angle += 2 * wv::Const::Double::PI;
	
	return angle;
}

template<typename Ty>
Vector2<Ty> slerp( const Vector2<Ty>& _a, const Vector2<Ty>& _b, double _t, bool _positive = false )
{
	Vector2<Ty> a = _a.normalized();
	Vector2<Ty> b = _b.normalized();

	// angle between normalized vector a and b
	double phi = angleBetween( a, b, _positive );

	const Ty tA = (Ty)std::sin( ( 1 - _t ) * phi ) / std::sin( phi );
	const Ty tB = (Ty)std::sin( _t * phi ) / std::sin( phi );

	return ( _a * tA ) + ( _b * tB );
}

}

///////////////////////////////////////////////////////////////////////////////////////

}