#pragma once

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

#include <stdint.h>
#include <type_traits>
#include <array>
#include <cstring>

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

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		cVector4<T>& right( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 0 ] ); }

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		cVector4<T>& up   ( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 1 ] ); }

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		cVector4<T>& at   ( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 2 ] ); }

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		cVector4<T>& pos  ( void ) { return *reinterpret_cast< cVector4<T>* >( m[ 3 ] ); }

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	cVector4<T> operator * ( const cMatrix<T, 4, 4>& _mat, const cVector4<T>& _vec )
	{
		cMatrix<T, 4, 1> tmpMat{};
		tmpMat[ 0 ][ 0 ] = _vec.x;
		tmpMat[ 1 ][ 0 ] = _vec.y;
		tmpMat[ 2 ][ 0 ] = _vec.z;
		tmpMat[ 3 ][ 0 ] = _vec.w;

		auto res = _mat * tmpMat;
		
		return { res[ 1 ][ 0 ], res[ 1 ][ 0 ], res[ 2 ][ 0 ], res[ 3 ][ 0 ] };
	}

	template<typename T>
	cVector4<T> operator * ( const cVector4<T>& _vec, const cMatrix<T, 4, 4>& _mat )
	{
		cMatrix<T, 1, 4> tmpMat{};
		tmpMat.setRow( 0, { _vec.x, _vec.y, _vec.z, _vec.z } );
		
		auto res = tmpMat * _mat;

		return { res[ 0 ][ 0 ], res[ 0 ][ 1 ], res[ 0 ][ 2 ], res[ 0 ][ 3 ] };
	}

///////////////////////////////////////////////////////////////////////////////////////

	namespace Matrix
	{

///////////////////////////////////////////////////////////////////////////////////////

		template<typename T, size_t RowA, size_t ColA_RowB, size_t ColB>
		cMatrix<T, RowA, ColB> multiply( const cMatrix<T, RowA, ColA_RowB>& _a, const cMatrix<T, ColA_RowB, ColB>& _b )
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
		static inline cMatrix<T, 4, 4> inverse( const cMatrix<T, 4, 4>& _m )
		{
			double A2323 = _m[ 2 ][ 2 ] * _m[ 3 ][ 3 ] - _m[ 2 ][ 3 ] * _m[ 3 ][ 2 ];
			double A1323 = _m[ 2 ][ 1 ] * _m[ 3 ][ 3 ] - _m[ 2 ][ 3 ] * _m[ 3 ][ 1 ];
			double A1223 = _m[ 2 ][ 1 ] * _m[ 3 ][ 2 ] - _m[ 2 ][ 2 ] * _m[ 3 ][ 1 ];
			double A0323 = _m[ 2 ][ 0 ] * _m[ 3 ][ 3 ] - _m[ 2 ][ 3 ] * _m[ 3 ][ 0 ];
			double A0223 = _m[ 2 ][ 0 ] * _m[ 3 ][ 2 ] - _m[ 2 ][ 2 ] * _m[ 3 ][ 0 ];
			double A0123 = _m[ 2 ][ 0 ] * _m[ 3 ][ 1 ] - _m[ 2 ][ 1 ] * _m[ 3 ][ 0 ];
			double A2313 = _m[ 1 ][ 2 ] * _m[ 3 ][ 3 ] - _m[ 1 ][ 3 ] * _m[ 3 ][ 2 ];
			double A1313 = _m[ 1 ][ 1 ] * _m[ 3 ][ 3 ] - _m[ 1 ][ 3 ] * _m[ 3 ][ 1 ];
			double A1213 = _m[ 1 ][ 1 ] * _m[ 3 ][ 2 ] - _m[ 1 ][ 2 ] * _m[ 3 ][ 1 ];
			double A2312 = _m[ 1 ][ 2 ] * _m[ 2 ][ 3 ] - _m[ 1 ][ 3 ] * _m[ 2 ][ 2 ];
			double A1312 = _m[ 1 ][ 1 ] * _m[ 2 ][ 3 ] - _m[ 1 ][ 3 ] * _m[ 2 ][ 1 ];
			double A1212 = _m[ 1 ][ 1 ] * _m[ 2 ][ 2 ] - _m[ 1 ][ 2 ] * _m[ 2 ][ 1 ];
			double A0313 = _m[ 1 ][ 0 ] * _m[ 3 ][ 3 ] - _m[ 1 ][ 3 ] * _m[ 3 ][ 0 ];
			double A0213 = _m[ 1 ][ 0 ] * _m[ 3 ][ 2 ] - _m[ 1 ][ 2 ] * _m[ 3 ][ 0 ];
			double A0312 = _m[ 1 ][ 0 ] * _m[ 2 ][ 3 ] - _m[ 1 ][ 3 ] * _m[ 2 ][ 0 ];
			double A0212 = _m[ 1 ][ 0 ] * _m[ 2 ][ 2 ] - _m[ 1 ][ 2 ] * _m[ 2 ][ 0 ];
			double A0113 = _m[ 1 ][ 0 ] * _m[ 3 ][ 1 ] - _m[ 1 ][ 1 ] * _m[ 3 ][ 0 ];
			double A0112 = _m[ 1 ][ 0 ] * _m[ 2 ][ 1 ] - _m[ 1 ][ 1 ] * _m[ 2 ][ 0 ];

			T det = _m[ 0 ][ 0 ] * ( _m[ 1 ][ 1 ] * A2323 - _m[ 1 ][ 2 ] * A1323 + _m[ 1 ][ 3 ] * A1223 )
				  - _m[ 0 ][ 1 ] * ( _m[ 1 ][ 0 ] * A2323 - _m[ 1 ][ 2 ] * A0323 + _m[ 1 ][ 3 ] * A0223 )
				  + _m[ 0 ][ 2 ] * ( _m[ 1 ][ 0 ] * A1323 - _m[ 1 ][ 1 ] * A0323 + _m[ 1 ][ 3 ] * A0123 )
				  - _m[ 0 ][ 3 ] * ( _m[ 1 ][ 0 ] * A1223 - _m[ 1 ][ 1 ] * A0223 + _m[ 1 ][ 2 ] * A0123 );

			if ( det == 0.0 ) // determinant is zero, inverse matrix does not exist
				return cMatrix<T, 4, 4>{};
			
			det = 1 / det;

			cMatrix<T, 4, 4> im;

			im[ 0 ][ 0 ] = det *  ( _m[ 1 ][ 1 ] * A2323 - _m[ 1 ][ 2 ] * A1323 + _m[ 1 ][ 3 ] * A1223 );
			im[ 0 ][ 1 ] = det * -( _m[ 0 ][ 1 ] * A2323 - _m[ 0 ][ 2 ] * A1323 + _m[ 0 ][ 3 ] * A1223 );
			im[ 0 ][ 2 ] = det *  ( _m[ 0 ][ 1 ] * A2313 - _m[ 0 ][ 2 ] * A1313 + _m[ 0 ][ 3 ] * A1213 );
			im[ 0 ][ 3 ] = det * -( _m[ 0 ][ 1 ] * A2312 - _m[ 0 ][ 2 ] * A1312 + _m[ 0 ][ 3 ] * A1212 );
			im[ 1 ][ 0 ] = det * -( _m[ 1 ][ 0 ] * A2323 - _m[ 1 ][ 2 ] * A0323 + _m[ 1 ][ 3 ] * A0223 );
			im[ 1 ][ 1 ] = det *  ( _m[ 0 ][ 0 ] * A2323 - _m[ 0 ][ 2 ] * A0323 + _m[ 0 ][ 3 ] * A0223 );
			im[ 1 ][ 2 ] = det * -( _m[ 0 ][ 0 ] * A2313 - _m[ 0 ][ 2 ] * A0313 + _m[ 0 ][ 3 ] * A0213 );
			im[ 1 ][ 3 ] = det *  ( _m[ 0 ][ 0 ] * A2312 - _m[ 0 ][ 2 ] * A0312 + _m[ 0 ][ 3 ] * A0212 );
			im[ 2 ][ 0 ] = det *  ( _m[ 1 ][ 0 ] * A1323 - _m[ 1 ][ 1 ] * A0323 + _m[ 1 ][ 3 ] * A0123 );
			im[ 2 ][ 1 ] = det * -( _m[ 0 ][ 0 ] * A1323 - _m[ 0 ][ 1 ] * A0323 + _m[ 0 ][ 3 ] * A0123 );
			im[ 2 ][ 2 ] = det *  ( _m[ 0 ][ 0 ] * A1313 - _m[ 0 ][ 1 ] * A0313 + _m[ 0 ][ 3 ] * A0113 );
			im[ 2 ][ 3 ] = det * -( _m[ 0 ][ 0 ] * A1312 - _m[ 0 ][ 1 ] * A0312 + _m[ 0 ][ 3 ] * A0112 );
			im[ 3 ][ 0 ] = det * -( _m[ 1 ][ 0 ] * A1223 - _m[ 1 ][ 1 ] * A0223 + _m[ 1 ][ 2 ] * A0123 );
			im[ 3 ][ 1 ] = det *  ( _m[ 0 ][ 0 ] * A1223 - _m[ 0 ][ 1 ] * A0223 + _m[ 0 ][ 2 ] * A0123 );
			im[ 3 ][ 2 ] = det * -( _m[ 0 ][ 0 ] * A1213 - _m[ 0 ][ 1 ] * A0213 + _m[ 0 ][ 2 ] * A0113 );
			im[ 3 ][ 3 ] = det *  ( _m[ 0 ][ 0 ] * A1212 - _m[ 0 ][ 1 ] * A0212 + _m[ 0 ][ 2 ] * A0112 );

			return im;
		}

		template<typename T, size_t R, size_t C>
		cMatrix<T, C, R> transpose( const cMatrix<T, R, C>& _m )
		{
			cMatrix<T, C, R> res;

			/// naive approach
			/// TODO: optimize
			for( size_t row = 0; row < R; row++ )
				for( size_t col = 0; col < C; col++ )
					res[ col ][ row ] = _m[ row ][ col ];
			
			return res;
		}

		template<typename T>
		cMatrix<T, 4, 4> translate( const cMatrix<T, 4, 4>& _m, const wv::cVector3<T>& _pos )
		{
			cMatrix<T, 4, 4> mat( T( 1 ) );
			
			mat.pos() = { _pos.x, _pos.y, _pos.z, T( 1 ) };

			return mat * _m;
		}

		template<typename T>
		cMatrix<T, 4, 4> scale( const cMatrix<T, 4, 4>& _m, const wv::cVector3<T>& _scale )
		{
			cMatrix<T, 4, 4> mat( 1.0 );
			
			mat.setRow( 0, { _scale.x,      0.0,      0.0 } );
			mat.setRow( 1, {      0.0, _scale.y,      0.0 } );
			mat.setRow( 2, {      0.0,      0.0, _scale.z } );

			return mat * _m;
		}

		template<typename T>
		cMatrix<T, 4, 4> rotateX( const cMatrix<T, 4, 4>& _m, T _angle )
		{
			cMatrix<T, 4, 4> mat( 1.0 );

			mat.setRow( 0, { 1.0,                0.0,                 0.0 } );
			mat.setRow( 1, { 0.0,  std::cos( _angle ), std::sin( _angle ) });
			mat.setRow( 2, { 0.0, -std::sin( _angle ), std::cos( _angle ) });

			return mat * _m;
		}

		template<typename T>
		cMatrix<T, 4, 4> rotateY( const cMatrix<T, 4, 4>& _m, T _angle )
		{
			cMatrix<T, 4, 4> mat( 1.0 );

			mat.setRow( 0, { std::cos( _angle ), 0.0, -std::sin( _angle ) } );
			mat.setRow( 1, {                0.0, 1.0,                 0.0 } );
			mat.setRow( 2, { std::sin( _angle ), 0.0,  std::cos( _angle ) } );

			return mat * _m;
		}

		template<typename T>
		cMatrix<T, 4, 4> rotateZ( const cMatrix<T, 4, 4>& _m, T _angle )
		{
			cMatrix<T, 4, 4> mat( 1.0 );

			mat.setRow( 0, {  std::cos( _angle ), std::sin( _angle ), 0.0 } );
			mat.setRow( 1, { -std::sin( _angle ), std::cos( _angle ), 0.0 } );
			mat.setRow( 2, {                 0.0,                0.0, 1.0 } );

			return mat * _m;
		}

		// https://jsantell.com/3d-projection/#field-of-view
		template<typename T>
		cMatrix<T, 4, 4> perspective( const T& _aspect, const T& _fov, const T& _near, const T& _far )
		{
			cMatrix<T, 4, 4> res{ 0 };

			const T e = 1.0 / std::tan( _fov / 2.0 );
			const T m00 = e / _aspect;
			const T m22 = ( _far + _near )     / ( _near - _far );
			const T m32 = ( 2 * _far * _near ) / ( _near - _far );

			res.setRow( 0, { m00, 0,   0,  0 } );
			res.setRow( 1, {   0, e,   0,  0 } );
			res.setRow( 2, {   0, 0, m22, -1 } );
			res.setRow( 3, {   0, 0, m32,  0 } );

			return res;
		}

		template<typename T>
		cMatrix<T, 4, 4> orthographic( const T& _halfWidth, const T& _halfHeight, const T& _far, const T& _near )
		{
			cMatrix<T, 4, 4> res{ 0 };

			T m00 = 1.0 / _halfWidth;
			T m11 = 1.0 / _halfHeight;
			T m22 = ( -2.0 ) / ( _far - _near );
			T m32 = -( ( _far + _near ) / ( _far - _near ) );

			res.setRow( 0, { m00,   0,   0, 0 } );
			res.setRow( 1, {   0, m11,   0, 0 } );
			res.setRow( 2, {   0,   0, m22, 0 } );
			res.setRow( 3, {   0,   0, m32, 1 } );

			return res;
		}

		/// TODO: focal length camera https://paulbourke.net/miscellaneous/lens/

		template<typename T>
		cMatrix<T, 1, 4> fromVector( const cVector4<T>& _vec )
		{
			cMatrix<T, 1, 4> m;
			m.setRow( 0, { _vec.x, _vec.y, _vec.z, _vec.w } );
			return m;
		}

		template<typename T>
		cMatrix<T, 1, 3> fromVector( cVector3<T> _vec )
		{
			cMatrix<T, 1, 3> m;
			m.setRow( 0, { _vec.x, _vec.y, _vec.z } );
			return m;
		}

	}

///////////////////////////////////////////////////////////////////////////////////////

	typedef cMatrix<float, 4, 4> cMatrix4x4f;
	typedef cMatrix<float, 3, 3> cMatrix3x3f;

///////////////////////////////////////////////////////////////////////////////////////
	
	template<typename T, size_t R, size_t C>
	inline cMatrix<T, R, C>& cMatrix<T, R, C>::operator=( const cMatrix<T, R, C>& _o )
	{
		memcpy( &m, &_o.m, sizeof( m ) );
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
		(*this) = (*this) * _o;
		return ( *this );
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