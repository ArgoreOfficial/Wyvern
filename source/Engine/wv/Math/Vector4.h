#pragma once

#include <cmath>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< typename T >
	class cVector4
	{

	public:

		T x, y, z, w;

///////////////////////////////////////////////////////////////////////////////////////

		cVector4( void )                                               : x{  0 }, y{  0 }, z{  0 }, w{  0 } { }
		cVector4( const T& _t )                                        : x{ _t }, y{ _t }, z{ _t }, w{ _t } { }
		cVector4( const T& _x, const T& _y, const T& _z, const T& _w ) : x{ _x }, y{ _y }, z{ _z }, w{ _w } { }


		T length( void )                      const { return std::sqrt( x * x + y * y + z * z + w * w ); }
		T dot   ( const cVector4<T>& _other ) const { return x * _other.x + y * _other.y + z * _other.z + w * _other.w; }

		void normalize( int _magnitude = 1.0f )
		{
			T magnitude = length();
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
			w /= magnitude;

			if ( _magnitude != 1.0f )
				*this *= _magnitude;
		}

		cVector4<T> normalized() const
		{
			cVector4<T> vec = *this;
			vec.normalize();
			return vec;
		}

		cVector4<T>& operator = ( const cVector4<T>& _other );
		cVector4<T>& operator +=( const cVector4<T>& _other );
		cVector4<T>  operator + ( const cVector4<T>& _other );
		cVector4<T>  operator * ( const float& _scalar );
		cVector4<T>& operator *=( const float& _scalar );
		cVector4<T>  operator / ( const float& _scalar );
		cVector4<T>& operator /=( const float& _scalar );

	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef cVector4<float>  cVector4f;
	typedef cVector4<double> cVector4d;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline cVector4<T>& cVector4<T>::operator=( const cVector4<T>& _other )
	{
		x = _other.x;
		y = _other.y;
		z = _other.z;
		w = _other.w;
		return *this;
	}

	template< typename T >
	inline cVector4<T>& cVector4<T>::operator+=( const cVector4<T>& _other )
	{
		x += _other.x;
		y += _other.y;
		z += _other.z;
		w += _other.w;
		return *this;
	}

	template< typename T >
	inline cVector4<T> cVector4<T>::operator+( const cVector4<T>& _other )
	{
		return cVector4<T>( x + _other.x, y + _other.y, z + _other.z, w + _other.w );
	}

	template< typename T >
	inline cVector4<T> wv::cVector4<T>::operator*( const float & _scalar )
	{
		return cVector4<T>( x * _scalar, y * _scalar, z * _scalar, w * _scalar );
	}

	template< typename T >
	inline cVector4<T>& cVector4<T>::operator*=( const float & _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		w *= _scalar;
		return *this;
	}

	template< typename T >
	inline cVector4<T> wv::cVector4<T>::operator/( const float & _scalar )
	{
		return cVector4<T>( x / _scalar, y / _scalar, z / _scalar, w / _scalar );
	}

	template< typename T >
	inline cVector4<T>& cVector4<T>::operator/=( const float & _scalar )
	{
		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		w /= _scalar;
		return *this;
	}

///////////////////////////////////////////////////////////////////////////////////////

}