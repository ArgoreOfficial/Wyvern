#pragma once

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< typename T >
	class Vector4
	{
	public:
		T x, y, z, w;

		Vector4( void )                                               : x{  0 }, y{  0 }, z{  0 }, w{  0 } { }
		Vector4( const T& _t )                                        : x{ _t }, y{ _t }, z{ _t }, w{ _t } { }
		Vector4( const T& _x, const T& _y, const T& _z, const T& _w ) : x{ _x }, y{ _y }, z{ _z }, w{ _w } { }

		Vector4<T>& operator = ( const Vector4<T>& _other );
		Vector4<T>& operator +=( const Vector4<T>& _other );
		Vector4<T>  operator + ( const Vector4<T>& _other );
		Vector4<T>  operator * ( const float& _scalar );
		Vector4<T>& operator *=( const float& _scalar );
		Vector4<T>  operator / ( const float& _scalar );
		Vector4<T>& operator /=( const float& _scalar );
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef Vector4< float > cVector4f;
	typedef Vector4< double > cVector4d;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline Vector4<T>& Vector4<T>::operator=( const Vector4<T>& _other )
	{
		x = _other.x;
		y = _other.y;
		z = _other.z;
		w = _other.w;
		return *this;
	}

	template< typename T >
	inline Vector4<T>& Vector4<T>::operator+=( const Vector4<T>& _other )
	{
		x += _other.x;
		y += _other.y;
		z += _other.z;
		w += _other.w;
		return *this;
	}

	template< typename T >
	inline Vector4<T> Vector4<T>::operator+( const Vector4<T>& _other )
	{
		return Vector4<T>( x + _other.x, y + _other.y, z + _other.z, w + _other.w );
	}

	template< typename T >
	inline Vector4<T> wv::Vector4<T>::operator*( const float & _scalar )
	{
		return Vector4<T>( x * _scalar, y * _scalar, z * _scalar, w * _scalar );
	}

	template< typename T >
	inline Vector4<T>& Vector4<T>::operator*=( const float & _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		w *= _scalar;
		return *this;
	}

	template< typename T >
	inline Vector4<T> wv::Vector4<T>::operator/( const float & _scalar )
	{
		return Vector4<T>( x / _scalar, y / _scalar, z / _scalar, w / _scalar );
	}

	template< typename T >
	inline Vector4<T>& Vector4<T>::operator/=( const float & _scalar )
	{
		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		w /= _scalar;
		return *this;
	}

///////////////////////////////////////////////////////////////////////////////////////

}