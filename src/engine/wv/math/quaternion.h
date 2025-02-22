#pragma once

#include <wv/math/vector3.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	class Quaternion
	{

	public:

		Vector3<T> v;
		T s;

///////////////////////////////////////////////////////////////////////////////////////

		Quaternion( void )                                               : v( {} ),         s( 1.0 ) { }
		Quaternion( const Vector3<T>& _v, const T& _s )                 : v( _v ),         s(  _s ) { }
		Quaternion( const T& _x, const T& _y, const T& _z, const T& _s ) : v( _x, _y, _z ), s(  _s ) { }

		static Vector3<T> rotateVector( const Vector3<T>& _v, const Vector3<T>& _axis, const T& _angle );

		static Quaternion<T> fromAxisAngle( const Vector3<T>& _axis, const T& _angle );

		void normalize ( void );
		void toUnitNorm( void );

		void rotate( const Vector3<T>& _axis, const T& _angle );

		T norm( void );

		Quaternion<T> normalized( void );
		Quaternion<T> conjugate ( void );
		Quaternion<T> inverse   ( void );


		Quaternion<T>& operator = ( const Quaternion<T>& _other );
		Quaternion<T>  operator + ( const Quaternion<T>& _other ) const;
		Quaternion<T>  operator - ( const Quaternion<T>& _other ) const;
		Quaternion<T>& operator +=( const Quaternion<T>& _other );
		Quaternion<T>& operator -=( const Quaternion<T>& _other );
		Quaternion<T>  operator * ( const Quaternion<T>& _other ) const;
		Quaternion<T>& operator *=( const Quaternion<T>& _other );
		Quaternion<T>  operator * ( const T& _scalar ) const;
		Quaternion<T>& operator *=( const T& _scalar );
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef Quaternion<float>  Quaternionf;
	typedef Quaternion<double> Quaterniond;
	typedef Quaternion<int>    Quaternioni;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline Vector3<T> Quaternion<T>::rotateVector( const Vector3<T>& _v, const Vector3<T>& _axis, const T& _angle )
	{
		Quaternion<T> pure{ _v, 0 };
		Vector3<T> axis = _axis;

		axis.normalize();

		Quaternion<T> q{ axis, _angle };
		q.toUnitNorm();

		Quaternion<T> qInv = q.inverse();
		Quaternion<T> rotated = q * pure * qInv;

		return rotated.v;
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::fromAxisAngle( const Vector3<T>& _axis, const T& _angle )
	{
		T factor = std::sin( _angle / 2.0 );

		T x = _axis.x * factor;
		T y = _axis.y * factor;
		T z = _axis.z * factor;

		T w = std::cos( _angle / 2.0 );

		return Quaternion<T>( x, y, z, w ).normalized();
	}

	template<typename T>
	inline void Quaternion<T>::normalize()
	{
		T n = norm();

		if( n == T{ 0 } )
			return;

		T normValue = T{ 1 } / n;
		s *= normValue;
		v *= normValue;
	}

	template<typename T>
	inline void Quaternion<T>::toUnitNorm()
	{
		T angle = Math::radians( s );
		v.normalize();
		s = std::cos( angle / T{ 2 } );
		v = v * std::sin( angle / T{ 2 } );
	}
	
	template<typename T>
	inline void Quaternion<T>::rotate( const Vector3<T>& _axis, const T& _angle )
	{
		Quaternion<T> _other = Quaternion<T>::fromAxisAngle( _axis, _angle );
		( *this ) *= _other;
	}

	template<typename T>
	inline T Quaternion<T>::norm()
	{
		T imaginary = v.dot( v );
		T scale = s * s;

		return sqrt( imaginary + scale );
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::normalized()
	{
		Quaternion<T> quat = ( *this );
		quat.normalize();
		return quat;
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::conjugate()
	{
		return Quaternion<T>( -v, s );
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::inverse()
	{
		T absoluteValue = norm();
		absoluteValue *= absoluteValue;
		absoluteValue = T{ 1 } / absoluteValue;

		Quaternion<T> conjugateValue = conjugate();
		T scale = conjugateValue.s * absoluteValue;

		Vector3<T> imaginary = conjugateValue.v * absoluteValue;
		return Quaternion<T>( imaginary, scale );
	}

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline Quaternion<T>& Quaternion<T>::operator=( const Quaternion<T>& _other )
	{
		v = _other.v;
		s = _other.s;

		return (*this);
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::operator+( const Quaternion<T>& _other ) const
	{
		Vector3<T> imaginary = v + _other.v;
		T scale              = s + _other.s;

		return Quaternion<T>{ imaginary, scale };
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::operator-( const Quaternion<T>& _other ) const
	{
		Vector3<T> imaginary = v - _other.v;
		T scale              = s - _other.s;

		return Quaternion<T>{ imaginary, scale };
	}

	template<typename T>
	inline Quaternion<T>& Quaternion<T>::operator+=( const Quaternion<T>& _other )
	{
		v += _other.v;
		s += _other.s;

		return (*this);
	}

	template<typename T>
	inline Quaternion<T>& Quaternion<T>::operator-=( const Quaternion<T>& _other )
	{
		v -= _other.v;
		s -= _other.s;

		return (*this);
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::operator*( const Quaternion<T>& _other ) const
	{
		Vector3<T> imaginary = ( _other.v * s ) + ( v * _other.s ) + v.cross( _other.v );
		T scale              = ( s * _other.s ) - v.dot( _other.v );

		return Quaternion<T>{ imaginary, scale };
	}

	template<typename T>
	inline Quaternion<T>& Quaternion<T>::operator*=( const Quaternion<T>& _other )
	{
		(*this) = (*this) * _other;

		return (*this);
	}

	template<typename T>
	inline Quaternion<T> Quaternion<T>::operator*( const T& _scalar ) const
	{
		Vector3<T> imaginary = v * _scalar;
		T scale              = s * _scalar;

		return Quaternion<T>( imaginary, scale );
	}

	template<typename T>
	inline Quaternion<T>& Quaternion<T>::operator*=( const T& _scalar )
	{
		v *= _scalar;
		s *= _scalar;

		return (*this);
	}

///////////////////////////////////////////////////////////////////////////////////////

}