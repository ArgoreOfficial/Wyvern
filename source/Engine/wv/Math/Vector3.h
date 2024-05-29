#pragma once

#include <cmath>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< typename T >
	class Vector3
	{
	public:
		T x, y, z;

		Vector3( void )                                  : x(  0 ), y(  0 ), z(  0 ) { }
		Vector3( const T& _t )                           : x( _t ), y( _t ), z( _t ) { }
		Vector3( const T& _x, const T& _y, const T& _z ) : x( _x ), y( _y ), z( _z ) { }

		T length( void )                      const { return std::sqrt( x * x + y * y + z * z ); }
		T dot   ( const Vector3<T>& _other ) const { return x * _other.x + y * _other.y + z * _other.z; }

		Vector3<T> cross( const Vector3<T>& _other ) const
		{
			return Vector3<T>(
				y * _other.z - z * _other.y,
				z * _other.x - x * _other.z,
				x * _other.y - y * _other.x
			);
		}

		void normalize( int _magnitude = 1.0f )
		{
			T magnitude = length();
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;

			if( _magnitude != 1.0f )
				*this *= _magnitude;
		}

		Vector3<T>& operator = ( const Vector3<T>& _other );
		Vector3<T>& operator +=( const Vector3<T>& _other );
		Vector3<T>& operator -=( const Vector3<T>& _other );
		Vector3<T>  operator + ( const Vector3<T>& _other );
		Vector3<T>  operator - ( const Vector3<T>& _other );
		Vector3<T>  operator - ( void );
		Vector3<T>  operator * ( const T& _scalar );
		Vector3<T>& operator *=( const T& _scalar );
		Vector3<T>  operator / ( const T& _scalar );
		Vector3<T>& operator /=( const T& _scalar );
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef Vector3< float > Vector3f;
	typedef Vector3< double > Vector3d;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline Vector3<T>& Vector3<T>::operator=( const Vector3<T>& _other )
	{
		x = _other.x;
		y = _other.y;
		z = _other.z;
		return *this;
	}

	template< typename T >
	inline Vector3<T>& Vector3<T>::operator+=( const Vector3<T>& _other )
	{
		x += _other.x;
		y += _other.y;
		z += _other.z;
		return *this;
	}

	template<typename T>
	inline Vector3<T>& Vector3<T>::operator-=( const Vector3<T>& _other )
	{
		x -= _other.x;
		y -= _other.y;
		z -= _other.z;
		return *this;
	}

	template< typename T >
	inline Vector3<T> Vector3<T>::operator+( const Vector3<T>& _other )
	{
		return Vector3<T>( x + _other.x, y + _other.y, z + _other.z );
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator-( const Vector3<T>& _other )
	{
		return Vector3<T>( x - _other.x, y - _other.y, z - _other.z );
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator-( void )
	{
		return Vector3<T>( -x, -y, -z );
	}

	template< typename T >
	inline Vector3<T> wv::Vector3<T>::operator*( const T& _scalar )
	{
		return Vector3<T>( x * _scalar, y * _scalar, z * _scalar );
	}

	template< typename T >
	inline Vector3<T>& wv::Vector3<T>::operator*=( const T& _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		return *this;
	}

	template< typename T >
	inline Vector3<T> wv::Vector3<T>::operator/( const T& _scalar )
	{
		return Vector3<T>( x / _scalar, y / _scalar, z / _scalar );
	}

	template< typename T >
	inline Vector3<T>& wv::Vector3<T>::operator/=( const T& _scalar )
	{
		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		return *this;
	}

///////////////////////////////////////////////////////////////////////////////////////

}