#pragma once

#include <cmath>
#include <wv/Math/Math.h>

#ifdef WV_CPP20
#include <compare>
#endif

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< typename T >
	class Vector3
	{

	public:

		T x, y, z;

///////////////////////////////////////////////////////////////////////////////////////

		Vector3( void )                                  : x(  0 ), y(  0 ), z(  0 ) { }
		Vector3( const T& _t )                           : x( _t ), y( _t ), z( _t ) { }
		Vector3( const T& _x, const T& _y, const T& _z ) : x( _x ), y( _y ), z( _z ) { }

		T length( void )                     const { return std::sqrt( x * x + y * y + z * z ); }
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

		Vector3<T> normalized()
		{
			Vector3 vec = *this;
			vec.normalize();
			return vec;
		}

		static inline Vector3<T> eulerToDirection( Vector3<T> _vec )
		{
			T pitch = wv::Math::degToRad( _vec.x );
			T yaw   = wv::Math::degToRad( _vec.y );
			
			return {
				 std::cos( pitch ) * std::sin( yaw ),
				-std::sin( pitch ),
				 std::cos( pitch ) * std::cos( yaw )
			};
		}

		static inline Vector3<T> directionToEuler( Vector3<T> _vec )
		{
			return {
				wv::Math::radToDeg( std::asin( -_vec.y ) ),         // pitch
				wv::Math::radToDeg( std::atan2( _vec.x, _vec.z ) ), // yaw
				0
			};
		}

		inline Vector3<T> eulerToDirection() { return Vector3<T>::eulerToDirection( *this ); }
		inline Vector3<T> directionToEuler() { return Vector3<T>::directionToEuler( *this ); }

		Vector3<T>& operator = ( const Vector3<T>& _other );
		Vector3<T>& operator +=( const Vector3<T>& _other );
		Vector3<T>& operator -=( const Vector3<T>& _other );
		Vector3<T>  operator + ( const Vector3<T>& _other ) const;
		Vector3<T>  operator - ( const Vector3<T>& _other ) const;
		Vector3<T>  operator - ( void ) const;
		Vector3<T>  operator * ( const T& _scalar ) const;
		Vector3<T>& operator *=( const T& _scalar );
		Vector3<T>  operator / ( const T& _scalar ) const;
		Vector3<T>& operator /=( const T& _scalar );

	#ifdef WV_CPP20
		auto operator<=>( const Vector3<T>& ) const = default;
	#endif
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef Vector3<float> Vector3f;
	typedef Vector3<double> Vector3d;

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
	inline Vector3<T> Vector3<T>::operator+( const Vector3<T>& _other ) const
	{
		return Vector3<T>( x + _other.x, y + _other.y, z + _other.z );
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator-( const Vector3<T>& _other ) const
	{
		return Vector3<T>( x - _other.x, y - _other.y, z - _other.z );
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator-( void ) const
	{
		return Vector3<T>( -x, -y, -z );
	}

	template< typename T >
	inline Vector3<T> wv::Vector3<T>::operator*( const T& _scalar ) const
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
	inline Vector3<T> wv::Vector3<T>::operator/( const T& _scalar ) const
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