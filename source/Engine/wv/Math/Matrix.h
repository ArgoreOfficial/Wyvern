#pragma once

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

#include <stdint.h>
#include <type_traits>
#include <array>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T, size_t R, size_t C>
	class cMatrix
	{
	public:

		typedef std::enable_if<R == 4 && C == 4> if_4x4;
		typedef std::enable_if<R == 3 && C == 3> if_3x3;
		typedef std::enable_if<R == C> if_square;

///////////////////////////////////////////////////////////////////////////////////////

		T m[ R ][ C ];

///////////////////////////////////////////////////////////////////////////////////////

		cMatrix( void ) : m{ 0 } { }
		cMatrix( const T& _val ) :
			m{ 0 }
		{
			static_assert( R == C, "Cannot create identity matrix from non-square matrix. See output" );
			for( size_t i = 0; i < R; i++ )
				m[ i ][ i ] = _val;
		}

///////////////////////////////////////////////////////////////////////////////////////

			                T*                operator []( const size_t& _index        ) const { return (T*)m[ _index ]; }
			                cMatrix<T, R, C>& operator = ( const cMatrix<T, R, C>&  _o );
		template<size_t C2> cMatrix<T, R, C2> operator * ( const cMatrix<T, C, C2>& _o ) const;
		template<size_t C2> cMatrix<T, R, C>& operator *=( const cMatrix<T, R, C2>& _o );

///////////////////////////////////////////////////////////////////////////////////////

		static cMatrix<T, R, R> identity( const T& _val ) 
		{
			return cMatrix<T, R, R>( _val ); 
		}

///////////////////////////////////////////////////////////////////////////////////////

		void setRow( const size_t& _r, std::array<T, C> _v );

		template<typename = if_4x4::type> cVector4<T>& right( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 0 ] ); }
		template<typename = if_4x4::type> cVector4<T>& up   ( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 1 ] ); }
		template<typename = if_4x4::type> cVector4<T>& at   ( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 2 ] ); }
		template<typename = if_4x4::type> cVector4<T>& pos  ( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 3 ] ); }

	};

///////////////////////////////////////////////////////////////////////////////////////

	namespace Matrix
	{

///////////////////////////////////////////////////////////////////////////////////////

		template<typename T, size_t RowA, size_t ColA_RowB, size_t ColB>
		cMatrix<T, RowA, ColB> multiply( cMatrix<T, RowA, ColA_RowB> _a, cMatrix<T, ColA_RowB, ColB> _b )
		{
			cMatrix<T, RowA, ColB> res;
			for( size_t row = 0; row < RowA; row++ )
			{
				for( size_t column = 0; column < ColB; column++ )
				{
					T v = 0;

					for( int inner = 0; inner < ColA_RowB; inner++ )
						v += _a.m[ row ][ inner ] * _b.m[ inner ][ column ];

					res[ row ][ column ] = v;
				}
			}
			return res;
		}

		template<typename T>
		cMatrix<T, 4, 4> translation( wv::cVector3<T> _pos )
		{
			cMatrix<T, 4, 4> mat( T( 1 ) );
			mat.pos() = { _pos.x, _pos.y, _pos.z, T( 1 ) };
			return mat;
		}

		template<typename T>
		cMatrix<T, 4, 4> scalar( wv::cVector3<T> _scale )
		{
			T zero = T( 0 );

			cMatrix<T, 4, 4> mat( T( 1 ) );
			
			mat.setRow( 0, { _scale.x,     zero,     zero } );
			mat.setRow( 1, {     zero, _scale.y,     zero } );
			mat.setRow( 2, {     zero,     zero, _scale.z } );

			return mat;
		}

	}

///////////////////////////////////////////////////////////////////////////////////////

	typedef cMatrix<float, 4, 4> cMatrix4x4f;

///////////////////////////////////////////////////////////////////////////////////////
	
	template<typename T, size_t R, size_t C>
	inline cMatrix<T, R, C>& cMatrix<T, R, C>::operator=( const cMatrix<T, R, C>& _o )
	{
		std::memcpy( &m, &_o.m, sizeof( m ) );
		return ( *this );
	}

	template<typename T, size_t R, size_t C>
	template<size_t C2>
	inline cMatrix<T, R, C2> cMatrix<T, R, C>::operator*( const cMatrix<T, C, C2>& _o ) const
	{
		return Matrix::multiply( *this, _o );
	}

	template<typename T, size_t R, size_t C>
	template<size_t C2>
	inline cMatrix<T, R, C>& cMatrix<T, R, C>::operator*=( const cMatrix<T, R, C2>& _o )
	{
		( *this ) = ( *this ) * _o;
	}

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T, size_t R, size_t C>
	inline void cMatrix<T, R, C>::setRow( const size_t& _r, std::array<T, C> _v )
	{
		size_t id = 0;
		for( auto& v : _v )
		{
			m[ _r ][ id ] = v;
			id++;
		}
	}

}