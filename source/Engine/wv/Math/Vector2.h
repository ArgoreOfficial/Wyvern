#pragma once

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template< typename T >
	class Vector2
	{

	public:

		T x, y;

///////////////////////////////////////////////////////////////////////////////////////

		Vector2( void ) : x( 0 ), y( 0 ){}
		Vector2( const T& _t ) : x( _t ), y( _t ){}
		Vector2( const T& _x, const T& _y ) : x( _x ), y( _y ){}

		Vector2<T>& operator = ( const Vector2<T>& _other );
		Vector2<T>& operator +=( const Vector2<T>& _other );
		Vector2<T>& operator -=( const Vector2<T>& _other );
		Vector2<T>  operator + ( const Vector2<T>& _other );
		Vector2<T>  operator - ( const Vector2<T>& _other );
		Vector2<T>  operator * ( const float& _scalar );
		Vector2<T>& operator *=( const float& _scalar );
		Vector2<T>  operator / ( const float& _scalar );
		Vector2<T>& operator /=( const float& _scalar );
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef Vector2< float > Vector2f;
	typedef Vector2< int > Vector2i;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline Vector2<T>& Vector2<T>::operator=( const Vector2<T>& _other )
	{
		x = _other.x;
		y = _other.y;
		return *this;
	}

	template< typename T >
	inline Vector2<T>& Vector2<T>::operator+=( const Vector2<T>& _other )
	{
		x += _other.x;
		y += _other.y;
		return *this;
	}

	template< typename T >
	inline Vector2<T>& Vector2<T>::operator-=( const Vector2<T>& _other )
	{
		x -= _other.x;
		y -= _other.y;
		return *this;
	}

	template< typename T >
	inline Vector2<T> Vector2<T>::operator+( const Vector2<T>& _other )
	{
		return Vector2<T>( x + _other.x, y + _other.y );
	}

	template< typename T >
	inline Vector2<T> Vector2<T>::operator-( const Vector2<T>& _other )
	{
		return Vector2<T>( x - _other.x, y - _other.y );
	}

	template< typename T >
	inline Vector2<T> wv::Vector2<T>::operator*( const float & _scalar )
	{
		return Vector2<T>( x * _scalar, y * _scalar );
	}

	template<typename T>
	inline Vector2<T> operator*(const float& scalar, const Vector2<T>& vec)
	{
		return Vector2<T>(vec.x * scalar, vec.y * scalar);
	}

	template< typename T >
	inline Vector2<T>& Vector2<T>::operator*=( const float & _scalar )
	{
		x *= _scalar;
		y *= _scalar;
		return *this;
	}
	
	template< typename T >
	inline Vector2<T> wv::Vector2<T>::operator/( const float & _scalar )
	{
		return Vector2<T>( x / _scalar, y / _scalar );
	}

	template< typename T >
	inline Vector2<T>& Vector2<T>::operator/=( const float & _scalar )
	{
		x /= _scalar;
		y /= _scalar;
		return *this;
	}



///////////////////////////////////////////////////////////////////////////////////////

}