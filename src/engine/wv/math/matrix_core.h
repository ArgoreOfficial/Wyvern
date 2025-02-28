#pragma once

#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <stdint.h>
#include <type_traits>
#include <array>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	template<typename T, size_t RowA, size_t ColA_RowB, size_t ColB>
	struct MatrixMult;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T, size_t R, size_t C>
	class Matrix
	{
	public:

		typedef std::enable_if<R == 4 && C == 4> if_4x4;
		typedef std::enable_if<R == 3 && C == 3> if_3x3;
		typedef std::enable_if<R == C> if_square;

///////////////////////////////////////////////////////////////////////////////////////

		T m[ R * C ];

///////////////////////////////////////////////////////////////////////////////////////

		Matrix( void ) : m{ 0 } { }
		Matrix( const T& _val ) :
			m{ 0 }
		{
			static_assert( R == C, "Cannot create identity matrix from non-square matrix. See output" );
			for( size_t i = 0; i < R; i++ )
				set( i, i, _val );
		}

		Matrix( const Matrix<T, R, C>& _other )
		{
			//for ( size_t r = 0; r < R; r++ )
			//	std::memcpy( m[ r ], _other.m[ r ], sizeof( T ) * C );
			std::memcpy( m, _other.m, sizeof( T ) * R * C );
		}

///////////////////////////////////////////////////////////////////////////////////////

			                T*               operator []( const size_t& _index        ) const { return (T*)m[ _index ]; }
			                Matrix<T, R, C>& operator = ( const Matrix<T, R, C>&  _o );
		template<size_t C2> Matrix<T, R, C2> operator * ( const Matrix<T, C, C2>& _o ) const;
		template<size_t C2> Matrix<T, R, C>& operator *=( const Matrix<T, R, C2>& _o );

///////////////////////////////////////////////////////////////////////////////////////

		static Matrix<T, R, R> identity( const T& _val ) 
		{
			return Matrix<T, R, R>( _val ); 
		}

///////////////////////////////////////////////////////////////////////////////////////

		inline void set( size_t _r, size_t _c, T _val ) {
			m[ _r * C + _c ] = _val;
		}

		inline T get( size_t _r, size_t _c ) const {
			return m[ _r * C + _c ];
		}

		void setRow( const size_t& _r, std::array<T, C> _v );

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		Vector4<T>& right( void ) { return *reinterpret_cast< Vector4<T>* >( &m[ 0 * 4 ] ); }

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		Vector4<T>& up   ( void ) { return *reinterpret_cast< Vector4<T>* >( &m[ 1 * 4 ] ); }

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		Vector4<T>& at   ( void ) { return *reinterpret_cast< Vector4<T>* >( &m[ 2 * 4 ] ); }

	#ifdef WV_CPP20
		template<typename = if_4x4::type>
	#endif
		Vector4<T>& pos  ( void ) { return *reinterpret_cast< Vector4<T>* >( &m[ 3 * 4 ] ); }

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	Vector4<T> operator*( const Matrix<T, 4, 4>& _mat, const Vector4<T>& _vec )
	{
		Matrix<T, 4, 1> tmpMat{};
		tmpMat.set( 0, 0, _vec.x );
		tmpMat.set( 1, 0, _vec.y );
		tmpMat.set( 2, 0, _vec.z );
		tmpMat.set( 3, 0, _vec.w );

		auto res = _mat * tmpMat;
		
		return { 
			res.get( 1, 0 ), 
			res.get( 1, 0 ), 
			res.get( 2, 0 ), 
			res.get( 3, 0 ) 
		};
	}

	template<typename T>
	Vector4<T> operator*( const Vector4<T>& _vec, const Matrix<T, 4, 4>& _mat )
	{
		Matrix<T, 1, 4> tmpMat{};
		tmpMat.setRow( 0, { _vec.x, _vec.y, _vec.z, _vec.z } );
		
		auto res = tmpMat * _mat;

		return { 
			res.get( 0, 0 ), 
			res.get( 0, 1 ), 
			res.get( 0, 2 ), 
			res.get( 0, 3 ) 
		};
	}

///////////////////////////////////////////////////////////////////////////////////////

	namespace MatrixUtil
	{

///////////////////////////////////////////////////////////////////////////////////////

		template<typename T, size_t RowA, size_t ColA_RowB, size_t ColB>
		Matrix<T, RowA, ColB> multiply( const Matrix<T, RowA, ColA_RowB>& _a, const Matrix<T, ColA_RowB, ColB>& _b )
		{
			return MatrixMult<T, RowA, ColA_RowB, ColB>::multiply( _a, _b );
		}

		template<typename T>
		static inline Matrix<T, 4, 4> inverse( const Matrix<T, 4, 4>& _m )
		{
			double A2323 = _m.get( 2, 2 ) * _m.get( 3, 3 ) - _m.get( 2, 3 ) * _m.get( 3, 2 );
			double A1323 = _m.get( 2, 1 ) * _m.get( 3, 3 ) - _m.get( 2, 3 ) * _m.get( 3, 1 );
			double A1223 = _m.get( 2, 1 ) * _m.get( 3, 2 ) - _m.get( 2, 2 ) * _m.get( 3, 1 );
			double A0323 = _m.get( 2, 0 ) * _m.get( 3, 3 ) - _m.get( 2, 3 ) * _m.get( 3, 0 );
			double A0223 = _m.get( 2, 0 ) * _m.get( 3, 2 ) - _m.get( 2, 2 ) * _m.get( 3, 0 );
			double A0123 = _m.get( 2, 0 ) * _m.get( 3, 1 ) - _m.get( 2, 1 ) * _m.get( 3, 0 );
			double A2313 = _m.get( 1, 2 ) * _m.get( 3, 3 ) - _m.get( 1, 3 ) * _m.get( 3, 2 );
			double A1313 = _m.get( 1, 1 ) * _m.get( 3, 3 ) - _m.get( 1, 3 ) * _m.get( 3, 1 );
			double A1213 = _m.get( 1, 1 ) * _m.get( 3, 2 ) - _m.get( 1, 2 ) * _m.get( 3, 1 );
			double A2312 = _m.get( 1, 2 ) * _m.get( 2, 3 ) - _m.get( 1, 3 ) * _m.get( 2, 2 );
			double A1312 = _m.get( 1, 1 ) * _m.get( 2, 3 ) - _m.get( 1, 3 ) * _m.get( 2, 1 );
			double A1212 = _m.get( 1, 1 ) * _m.get( 2, 2 ) - _m.get( 1, 2 ) * _m.get( 2, 1 );
			double A0313 = _m.get( 1, 0 ) * _m.get( 3, 3 ) - _m.get( 1, 3 ) * _m.get( 3, 0 );
			double A0213 = _m.get( 1, 0 ) * _m.get( 3, 2 ) - _m.get( 1, 2 ) * _m.get( 3, 0 );
			double A0312 = _m.get( 1, 0 ) * _m.get( 2, 3 ) - _m.get( 1, 3 ) * _m.get( 2, 0 );
			double A0212 = _m.get( 1, 0 ) * _m.get( 2, 2 ) - _m.get( 1, 2 ) * _m.get( 2, 0 );
			double A0113 = _m.get( 1, 0 ) * _m.get( 3, 1 ) - _m.get( 1, 1 ) * _m.get( 3, 0 );
			double A0112 = _m.get( 1, 0 ) * _m.get( 2, 1 ) - _m.get( 1, 1 ) * _m.get( 2, 0 );

			T det = _m.get( 0, 0 ) * ( _m.get( 1, 1 ) * A2323 - _m.get( 1, 2 ) * A1323 + _m.get( 1, 3 ) * A1223 )
				  - _m.get( 0, 1 ) * ( _m.get( 1, 0 ) * A2323 - _m.get( 1, 2 ) * A0323 + _m.get( 1, 3 ) * A0223 )
				  + _m.get( 0, 2 ) * ( _m.get( 1, 0 ) * A1323 - _m.get( 1, 1 ) * A0323 + _m.get( 1, 3 ) * A0123 )
				  - _m.get( 0, 3 ) * ( _m.get( 1, 0 ) * A1223 - _m.get( 1, 1 ) * A0223 + _m.get( 1, 2 ) * A0123 );

			if ( det == 0.0 ) // determinant is zero, inverse matrix does not exist
				return Matrix<T, 4, 4>{};
			
			det = 1 / det;

			Matrix<T, 4, 4> im;

			im.set( 0, 0, det *  ( _m.get( 1, 1 ) * A2323 - _m.get( 1, 2 ) * A1323 + _m.get( 1, 3 ) * A1223 ) );
			im.set( 0, 1, det * -( _m.get( 0, 1 ) * A2323 - _m.get( 0, 2 ) * A1323 + _m.get( 0, 3 ) * A1223 ) );
			im.set( 0, 2, det *  ( _m.get( 0, 1 ) * A2313 - _m.get( 0, 2 ) * A1313 + _m.get( 0, 3 ) * A1213 ) );
			im.set( 0, 3, det * -( _m.get( 0, 1 ) * A2312 - _m.get( 0, 2 ) * A1312 + _m.get( 0, 3 ) * A1212 ) );
			im.set( 1, 0, det * -( _m.get( 1, 0 ) * A2323 - _m.get( 1, 2 ) * A0323 + _m.get( 1, 3 ) * A0223 ) );
			im.set( 1, 1, det *  ( _m.get( 0, 0 ) * A2323 - _m.get( 0, 2 ) * A0323 + _m.get( 0, 3 ) * A0223 ) );
			im.set( 1, 2, det * -( _m.get( 0, 0 ) * A2313 - _m.get( 0, 2 ) * A0313 + _m.get( 0, 3 ) * A0213 ) );
			im.set( 1, 3, det *  ( _m.get( 0, 0 ) * A2312 - _m.get( 0, 2 ) * A0312 + _m.get( 0, 3 ) * A0212 ) );
			im.set( 2, 0, det *  ( _m.get( 1, 0 ) * A1323 - _m.get( 1, 1 ) * A0323 + _m.get( 1, 3 ) * A0123 ) );
			im.set( 2, 1, det * -( _m.get( 0, 0 ) * A1323 - _m.get( 0, 1 ) * A0323 + _m.get( 0, 3 ) * A0123 ) );
			im.set( 2, 2, det *  ( _m.get( 0, 0 ) * A1313 - _m.get( 0, 1 ) * A0313 + _m.get( 0, 3 ) * A0113 ) );
			im.set( 2, 3, det * -( _m.get( 0, 0 ) * A1312 - _m.get( 0, 1 ) * A0312 + _m.get( 0, 3 ) * A0112 ) );
			im.set( 3, 0, det * -( _m.get( 1, 0 ) * A1223 - _m.get( 1, 1 ) * A0223 + _m.get( 1, 2 ) * A0123 ) );
			im.set( 3, 1, det *  ( _m.get( 0, 0 ) * A1223 - _m.get( 0, 1 ) * A0223 + _m.get( 0, 2 ) * A0123 ) );
			im.set( 3, 2, det * -( _m.get( 0, 0 ) * A1213 - _m.get( 0, 1 ) * A0213 + _m.get( 0, 2 ) * A0113 ) );
			im.set( 3, 3, det *  ( _m.get( 0, 0 ) * A1212 - _m.get( 0, 1 ) * A0212 + _m.get( 0, 2 ) * A0112 ) );

			return im;
		}

		template<typename T, size_t R, size_t C>
		Matrix<T, C, R> transpose( const Matrix<T, R, C>& _m )
		{
			Matrix<T, C, R> res;

			/// naive approach
			/// TODO: optimize
			for( size_t row = 0; row < R; row++ )
				for( size_t col = 0; col < C; col++ )
					res.set( col, row, _m.get( row, col ) );
			
			return res;
		}

		template<typename T>
		Matrix<T, 4, 4> translate( const Matrix<T, 4, 4>& _m, const wv::Vector3<T>& _pos )
		{
			Matrix<T, 4, 4> mat( T( 1 ) );
			
			mat.pos() = { _pos.x, _pos.y, _pos.z, T( 1 ) };

			return mat * _m;
		}

		template<typename T>
		Matrix<T, 4, 4> scale( const Matrix<T, 4, 4>& _m, const wv::Vector3<T>& _scale )
		{
			Matrix<T, 4, 4> mat( 1.0 );
			
			mat.setRow( 0, { _scale.x,      0.0,      0.0 } );
			mat.setRow( 1, {      0.0, _scale.y,      0.0 } );
			mat.setRow( 2, {      0.0,      0.0, _scale.z } );

			return mat * _m;
		}

		template<typename T>
		Matrix<T, 4, 4> rotateX( const Matrix<T, 4, 4>& _m, T _angle )
		{
			Matrix<T, 4, 4> mat( 1.0 );

			mat.setRow( 0, { 1.0,                0.0,                 0.0 } );
			mat.setRow( 1, { 0.0,  std::cos( _angle ), std::sin( _angle ) });
			mat.setRow( 2, { 0.0, -std::sin( _angle ), std::cos( _angle ) });

			return mat * _m;
		}

		template<typename T>
		Matrix<T, 4, 4> rotateY( const Matrix<T, 4, 4>& _m, T _angle )
		{
			Matrix<T, 4, 4> mat( 1.0 );

			mat.setRow( 0, { std::cos( _angle ), 0.0, -std::sin( _angle ) } );
			mat.setRow( 1, {                0.0, 1.0,                 0.0 } );
			mat.setRow( 2, { std::sin( _angle ), 0.0,  std::cos( _angle ) } );

			return mat * _m;
		}

		template<typename T>
		Matrix<T, 4, 4> rotateZ( const Matrix<T, 4, 4>& _m, T _angle )
		{
			Matrix<T, 4, 4> mat( 1.0 );

			mat.setRow( 0, {  std::cos( _angle ), std::sin( _angle ), 0.0 } );
			mat.setRow( 1, { -std::sin( _angle ), std::cos( _angle ), 0.0 } );
			mat.setRow( 2, {                 0.0,                0.0, 1.0 } );

			return mat * _m;
		}

		// https://jsantell.com/3d-projection/#field-of-view
		template<typename T>
		Matrix<T, 4, 4> perspective( const T& _aspect, const T& _fov, const T& _near, const T& _far )
		{
			Matrix<T, 4, 4> res{ 0 };

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
		Matrix<T, 4, 4> orthographic( const T& _halfWidth, const T& _halfHeight, const T& _far, const T& _near )
		{
			Matrix<T, 4, 4> res{ 0 };

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
		Matrix<T, 1, 4> fromVector( const Vector4<T>& _vec )
		{
			Matrix<T, 1, 4> m;
			m.setRow( 0, { _vec.x, _vec.y, _vec.z, _vec.w } );
			return m;
		}

		template<typename T>
		Matrix<T, 1, 3> fromVector( Vector3<T> _vec )
		{
			Matrix<T, 1, 3> m;
			m.setRow( 0, { _vec.x, _vec.y, _vec.z } );
			return m;
		}

	}

///////////////////////////////////////////////////////////////////////////////////////

	typedef Matrix<float, 4, 4> Matrix4x4f;
	typedef Matrix<float, 3, 3> Matrix3x3f;

///////////////////////////////////////////////////////////////////////////////////////
	
	template<typename T, size_t R, size_t C>
	inline Matrix<T, R, C>& Matrix<T, R, C>::operator=( const Matrix<T, R, C>& _o )
	{
		std::memcpy( &m, &_o.m, sizeof( m ) );
		return ( *this );
	}

	template<typename T, size_t R, size_t C>
	template<size_t C2>
	inline Matrix<T, R, C2> Matrix<T, R, C>::operator*( const Matrix<T, C, C2>& _o ) const
	{
		return MatrixUtil::multiply( *this, _o );
	}

	template<typename T, size_t R, size_t C>
	template<size_t C2>
	inline Matrix<T, R, C>& Matrix<T, R, C>::operator*=( const Matrix<T, R, C2>& _o )
	{
		(*this) = (*this) * _o;
		return ( *this );
	}

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T, size_t R, size_t C>
	inline void Matrix<T, R, C>::setRow( const size_t& _r, std::array<T, C> _v )
	{
		size_t id = 0;
		for( auto& v : _v )
		{
			set( _r, id, v );
			id++;
		}
	}

}