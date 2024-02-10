#pragma once

#include <cmath>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< typename T >
	class cVector3
	{
	public:
		T x, y, z;

		cVector3( void ) : x( 0 ), y( 0 ), z( 0 ){}
		cVector3( const T& _t ) : x( _t ), y( _t ), z( _t ){}
		cVector3( const T& _x, const T& _y, const T& _z ) : x( _x ), y( _y ), z( _z ){}

		T length() { return std::sqrt( x * x + y * y + z * z ); }

		cVector3< T >& operator = ( const cVector3< T >& _other );
		cVector3< T >& operator +=( const cVector3< T >& _other );
		cVector3< T >& operator -=( const cVector3< T >& _other );
		cVector3< T >  operator + ( const cVector3< T >& _other );
		cVector3< T >  operator - ( const cVector3< T >& _other );
		cVector3< T >  operator * ( const T& _scalar );
		cVector3< T >& operator *=( const T& _scalar );
		cVector3< T >  operator / ( const T& _scalar );
		cVector3< T >& operator /=( const T& _scalar );
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef cVector3< float > cVector3f;
	typedef cVector3< double > cVector3d;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline cVector3< T >& cVector3< T >::operator=( const cVector3< T >& _other )
	{
		x = _other.x;
		y = _other.y;
		z = _other.z;
		return *this;
	}

	template< typename T >
	inline cVector3< T >& cVector3< T >::operator+=( const cVector3< T >& _other )
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
	inline cVector3< T > cVector3< T >::operator+( const cVector3< T >& _other )
	{
		return cVector3< T >( x + _other.x, y + _other.y, z + _other.z );
	}

	template<typename T>
	inline cVector3<T> cVector3<T>::operator-( const cVector3<T>& _other )
	{
		return cVector3< T >( x - _other.x, y - _other.y, z - _other.z );
	}

	template< typename T >
	inline cVector3< T > wv::cVector3< T >::operator*( const T& _scalar )
	{
		return cVector3< T >( x * _scalar, y * _scalar, z * _scalar );
	}

	template< typename T >
	inline cVector3< T >& wv::cVector3< T >::operator*=( const T& _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		return *this;
	}

	template< typename T >
	inline cVector3< T > wv::cVector3< T >::operator/( const T& _scalar )
	{
		return cVector3< T >( x / _scalar, y / _scalar, z / _scalar );
	}

	template< typename T >
	inline cVector3< T >& wv::cVector3< T >::operator/=( const T& _scalar )
	{
		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		return *this;
	}

///////////////////////////////////////////////////////////////////////////////////////

}