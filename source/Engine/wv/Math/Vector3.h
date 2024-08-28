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
	class cVector3
	{

	public:

		T x, y, z;

///////////////////////////////////////////////////////////////////////////////////////

		cVector3( void )                                  : x(  0 ), y(  0 ), z(  0 ) { }
		cVector3( const T& _t )                           : x( _t ), y( _t ), z( _t ) { }
		cVector3( const T& _x, const T& _y, const T& _z ) : x( _x ), y( _y ), z( _z ) { }

		T length( void )                      const { return std::sqrt( x * x + y * y + z * z ); }
		T dot   ( const cVector3<T>& _other ) const { return x * _other.x + y * _other.y + z * _other.z; }

		cVector3<T> cross( const cVector3<T>& _other ) const
		{
			return cVector3<T>(
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

		cVector3<T> normalized()
		{
			cVector3 vec = *this;
			vec.normalize();
			return vec;
		}

		static inline cVector3<T> eulerToDirection( cVector3<T> _vec )
		{
			T pitch = wv::Math::radians( _vec.x );
			T yaw   = wv::Math::radians( _vec.y );
			
			return {
				 std::cos( pitch ) * std::sin( yaw ),
				-std::sin( pitch ),
				 std::cos( pitch ) * std::cos( yaw )
			};
		}

		static inline cVector3<T> directionToEuler( cVector3<T> _vec )
		{
			return {
				wv::Math::radians( std::asin( -_vec.y ) ),         // pitch
				wv::Math::radians( std::atan2( _vec.x, _vec.z ) ), // yaw
				0
			};
		}

		inline cVector3<T> eulerToDirection() { return cVector3<T>::eulerToDirection( *this ); }
		inline cVector3<T> directionToEuler() { return cVector3<T>::directionToEuler( *this ); }

		cVector3<T>& operator = ( const cVector3<T>& _other );
		cVector3<T>& operator +=( const cVector3<T>& _other );
		cVector3<T>& operator -=( const cVector3<T>& _other );
		cVector3<T>  operator + ( const cVector3<T>& _other ) const;
		cVector3<T>  operator - ( const cVector3<T>& _other ) const;
		cVector3<T>  operator - ( void ) const;
		cVector3<T>  operator * ( const T& _scalar ) const;
		cVector3<T>& operator *=( const T& _scalar );
		cVector3<T>  operator / ( const T& _scalar ) const;
		cVector3<T>& operator /=( const T& _scalar );

	#ifdef WV_CPP20
		auto operator<=>( const cVector3<T>& ) const = default;
	#endif
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef cVector3<float>  cVector3f;
	typedef cVector3<double> cVector3d;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline cVector3<T>& cVector3<T>::operator=( const cVector3<T>& _other )
	{
		x = _other.x;
		y = _other.y;
		z = _other.z;
		return *this;
	}

	template< typename T >
	inline cVector3<T>& cVector3<T>::operator+=( const cVector3<T>& _other )
	{
		x += _other.x;
		y += _other.y;
		z += _other.z;
		return *this;
	}

	template<typename T>
	inline cVector3<T>& cVector3<T>::operator-=( const cVector3<T>& _other )
	{
		x -= _other.x;
		y -= _other.y;
		z -= _other.z;
		return *this;
	}

	template< typename T >
	inline cVector3<T> cVector3<T>::operator+( const cVector3<T>& _other ) const
	{
		return cVector3<T>( x + _other.x, y + _other.y, z + _other.z );
	}

	template<typename T>
	inline cVector3<T> cVector3<T>::operator-( const cVector3<T>& _other ) const
	{
		return cVector3<T>( x - _other.x, y - _other.y, z - _other.z );
	}

	template<typename T>
	inline cVector3<T> cVector3<T>::operator-( void ) const
	{
		return cVector3<T>( -x, -y, -z );
	}

	template< typename T >
	inline cVector3<T> wv::cVector3<T>::operator*( const T& _scalar ) const
	{
		return cVector3<T>( x * _scalar, y * _scalar, z * _scalar );
	}

	template< typename T >
	inline cVector3<T>& wv::cVector3<T>::operator*=( const T& _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		return *this;
	}

	template< typename T >
	inline cVector3<T> wv::cVector3<T>::operator/( const T& _scalar ) const
	{
		return cVector3<T>( x / _scalar, y / _scalar, z / _scalar );
	}

	template< typename T >
	inline cVector3<T>& wv::cVector3<T>::operator/=( const T& _scalar )
	{
		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		return *this;
	}

///////////////////////////////////////////////////////////////////////////////////////

}