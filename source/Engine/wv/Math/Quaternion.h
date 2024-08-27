#pragma once

#include <wv/Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	class cQuaternion
	{

	public:

		cVector3<T> v;
		T s;

///////////////////////////////////////////////////////////////////////////////////////

		cQuaternion( void )                                               : v( {} ),         s(  0 ) { }
		cQuaternion( const cVector3<T>& _v, const T& _s )                 : v( _v ),         s( _s ) { }
		cQuaternion( const T& _x, const T& _y, const T& _z, const T& _s ) : v( _x, _y, _z ), s( _s ) { }

		static cVector3<T> rotateVector( const cVector3<T>& _v, const cVector3<T>& _axis, const T& _angle );

		void normalize ( void );
		void toUnitNorm( void );

		T norm( void );

		cQuaternion<T> normalized( void );
		cQuaternion<T> conjugate ( void );
		cQuaternion<T> inverse   ( void );

		cQuaternion<T>& operator = ( const cQuaternion<T>& _other );
		cQuaternion<T>  operator + ( const cQuaternion<T>& _other ) const;
		cQuaternion<T>  operator - ( const cQuaternion<T>& _other ) const;
		cQuaternion<T>& operator +=( const cQuaternion<T>& _other );
		cQuaternion<T>& operator -=( const cQuaternion<T>& _other );
		cQuaternion<T>  operator * ( const cQuaternion<T>& _other ) const;
		cQuaternion<T>& operator *=( const cQuaternion<T>& _other );
		cQuaternion<T>  operator * ( const T& _scalar ) const;
		cQuaternion<T>& operator *=( const T& _scalar );
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef cQuaternion<float>  cQuaternionf;
	typedef cQuaternion<double> cQuaterniond;
	typedef cQuaternion<int>    cQuaternioni;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline cVector3<T> cQuaternion<T>::rotateVector( const cVector3<T>& _v, const cVector3<T>& _axis, const T& _angle )
	{
		cQuaternion<T> pure{ _v, 0 };
		cVector3<T> axis = _axis;

		axis.normalize();

		cQuaternion<T> q{ axis, _angle };
		q.toUnitNorm();

		cQuaternion<T> qInv = q.inverse();
		cQuaternion<T> rotated = q * pure * qInv;

		return rotated.v;
	}

	template<typename T>
	inline void cQuaternion<T>::normalize()
	{
		T n = norm();

		if( n == T{ 0 } )
			return;

		T normValue = T{ 1 } / n;
		s *= normValue;
		v *= normValue;
	}

	template<typename T>
	inline void cQuaternion<T>::toUnitNorm()
	{
		T angle = Math::degToRad( s );
		v.normalize();
		s = std::cos( angle / T{ 2 } );
		v = v * std::sin( angle / T{ 2 } );
	}

	template<typename T>
	inline T cQuaternion<T>::norm()
	{
		T imaginary = v.dot( v );
		T scale = s * s;

		return sqrt( imaginary + scale );
	}

	template<typename T>
	inline cQuaternion<T> cQuaternion<T>::normalized()
	{
		cQuaternion<T> quat = ( *this );
		quat.normalize();
		return quat;
	}

	template<typename T>
	inline cQuaternion<T> cQuaternion<T>::conjugate()
	{
		return cQuaternion<T>( -v, s );
	}

	template<typename T>
	inline cQuaternion<T> cQuaternion<T>::inverse()
	{
		T absoluteValue = norm();
		absoluteValue *= absoluteValue;
		absoluteValue = T{ 1 } / absoluteValue;

		cQuaternion<T> conjugateValue = conjugate();
		T scale = conjugateValue.s * absoluteValue;

		cVector3<T> imaginary = conjugateValue.v * absoluteValue;
		return cQuaternion<T>( imaginary, scale );
	}

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline cQuaternion<T>& cQuaternion<T>::operator=( const cQuaternion<T>& _other )
	{
		v = _other.v;
		s = _other.s;

		return (*this);
	}

	template<typename T>
	inline cQuaternion<T> cQuaternion<T>::operator+( const cQuaternion<T>& _other ) const
	{
		cVector3<T> imaginary = v + _other.v;
		T scale              = s + _other.s;

		return cQuaternion<T>{ imaginary, scale };
	}

	template<typename T>
	inline cQuaternion<T> cQuaternion<T>::operator-( const cQuaternion<T>& _other ) const
	{
		cVector3<T> imaginary = v - _other.v;
		T scale              = s - _other.s;

		return cQuaternion<T>{ imaginary, scale };
	}

	template<typename T>
	inline cQuaternion<T>& cQuaternion<T>::operator+=( const cQuaternion<T>& _other )
	{
		v += _other.v;
		s += _other.s;

		return (*this);
	}

	template<typename T>
	inline cQuaternion<T>& cQuaternion<T>::operator-=( const cQuaternion<T>& _other )
	{
		v -= _other.v;
		s -= _other.s;

		return (*this);
	}

	template<typename T>
	inline cQuaternion<T> cQuaternion<T>::operator*( const cQuaternion<T>& _other ) const
	{
		cVector3<T> imaginary = ( _other.v * s ) + ( v * _other.s ) + v.cross( _other.v );
		T scale              = ( s * _other.s ) - v.dot( _other.v );

		return cQuaternion<T>{ imaginary, scale };
	}

	template<typename T>
	inline cQuaternion<T>& cQuaternion<T>::operator*=( const cQuaternion<T>& _other )
	{
		(*this) = (*this) * _other;

		return (*this);
	}

	template<typename T>
	inline cQuaternion<T> cQuaternion<T>::operator*( const T& _scalar ) const
	{
		cVector3<T> imaginary = v * _scalar;
		T scale              = s * _scalar;

		return cQuaternion<T>( imaginary, scale );
	}

	template<typename T>
	inline cQuaternion<T>& cQuaternion<T>::operator*=( const T& _scalar )
	{
		v *= _scalar;
		s *= _scalar;

		return (*this);
	}

///////////////////////////////////////////////////////////////////////////////////////

}