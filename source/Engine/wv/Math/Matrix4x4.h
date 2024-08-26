#pragma once

#include <stdio.h>

#include <wv/Math/Matrix.h>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

namespace wv
{
	template<typename T>
	class cMatrix4x4
	{
	public:
		Matrix::sPureMatrix<T, 4, 4> m;
		
		cMatrix4x4() : m{} { };
		cMatrix4x4( const T& _s ) :
			m{ Matrix::identity<T, 4>( _s ) }
		{ };
		
		cVector4<T>& right( void ) { return *reinterpret_cast<cVector4<T>*>( m[ 0 ] ); }
		cVector4<T>& up   ( void ) { return *reinterpret_cast<cVector4<T>*>( m[ 1 ] ); }
		cVector4<T>& at   ( void ) { return *reinterpret_cast<cVector4<T>*>( m[ 2 ] ); }
		cVector4<T>& pos  ( void ) { return *reinterpret_cast<cVector4<T>*>( m[ 3 ] ); }

		cMatrix4x4<T>& operator = ( const cMatrix4x4<T>& _o );
		cMatrix4x4<T>  operator * ( const cMatrix4x4<T>& _o ) const;
		cMatrix4x4<T>& operator *=( const cMatrix4x4<T>& _o );
	};
	
	template<typename T>
	inline cMatrix4x4<T>& cMatrix4x4<T>::operator=( const cMatrix4x4<T>& _o )
	{
		memcpy( &m, &_o.m, sizeof( m ) );
	}
	
	template<typename T>
	inline cMatrix4x4<T> cMatrix4x4<T>::operator*( const cMatrix4x4<T>& _o ) const
	{
		return Matrix::pureMult( m, _o.m );
	}

	template<typename T>
	inline cMatrix4x4<T>& cMatrix4x4<T>::operator*=( const cMatrix4x4<T>& _o )
	{
		( *this ) = ( *this ) * _o;
	}
}