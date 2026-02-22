#pragma once

#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <stdint.h>
#include <type_traits>
#include <array>
#include <cstring>

#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {
template<typename Ty, size_t RowA, size_t ColARowB, size_t ColB>
struct MatrixMult;

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty, size_t Row, size_t Col>
class Matrix
{
public:

	typedef std::enable_if<Row == 4 && Col == 4> if_4x4;
	typedef std::enable_if<Row == Col> if_square;

///////////////////////////////////////////////////////////////////////////////////////

	Ty m[ Row * Col ];

///////////////////////////////////////////////////////////////////////////////////////

	Matrix( void ) : m{ 0 } {}
	Matrix( const Ty& _val ) :
		m{ 0 }
	{
		static_assert( Row == Col, "Cannot create identity matrix from non-square matrix. See output" );

		for( size_t i = 0; i < Row; i++ )
			set( i, i, _val );
	}

	Matrix( const Matrix<Ty, Row, Col>& _other )
	{
		std::memcpy( m, _other.m, sizeof( m ) );
	}

	Matrix( const Ty( &_list )[ Row * Col ] )
	{
		std::memcpy( m, _list, sizeof( m ) );
	}

	Matrix( const Ty* _val )
	{
		std::memcpy( m, _val, sizeof( m ) );
	}

	Matrix( const Vector3<Ty>& _vec ):
		m{ 0 }
	{
		static_assert( Row == 1 && Col == 3, "Only Matrix<1, 3> can be constructed from Vector3" );

		setRow( 0, { _vec.x, _vec.y, _vec.z } );
	}

	Matrix( const Vector4<Ty>& _vec ):
		m{ 0 }
	{
		static_assert( Row == 1 && Col == 4, "Only Matrix<1, 4> can be constructed from Vector4" );

		setRow( 0, { _vec.x, _vec.y, _vec.z, _vec.w } );
	}

///////////////////////////////////////////////////////////////////////////////////////

	Ty* operator []( const size_t& _index ) const { 
		return (Ty*)m[ _index ]; 
	}

	                      Matrix<Ty, Row, Col>& operator = ( const Matrix<Ty, Row, Col>&  _o );
	template<size_t ColB> Matrix<Ty, Row, ColB> operator * ( const Matrix<Ty, Col, ColB>& _o ) const;
	template<size_t ColB> Matrix<Ty, Row, Col>& operator *=( const Matrix<Ty, Row, ColB>& _o );

///////////////////////////////////////////////////////////////////////////////////////

	static Matrix<Ty, Row, Row> identity( const Ty& _val )
	{
		return Matrix<Ty, Row, Row>( _val );
	}

///////////////////////////////////////////////////////////////////////////////////////

	bool isIdentity() const {
		if constexpr ( Row != Col ) return false; // must be square

		for ( size_t c = 0; c < Col; c++ )
		{
			for ( size_t r = 0; r < Row; r++ )
			{
				if ( get( r, c ) == ( c == r ? 1.0 : 0.0 ) )
					continue;
				else
					return false;
			}
		}

		return true;
	}

	inline void set( size_t _r, size_t _c, Ty _val ) {
	#ifdef __cpp_exceptions
		if( _r >= Row || _c >= Col )
			throw std::runtime_error( "out of range row or column" );
	#endif
		m[ _r * Col + _c ] = _val;
	}

	inline Ty get( size_t _r, size_t _c ) const {
	#ifdef __cpp_exceptions
		if( _r >= Row || _c >= Col )
			throw std::runtime_error( "out of range row or column" );
	#endif
		return m[ _r * Col + _c ];
	}

	void setRow( const size_t& _r, std::array<Ty, Col> _v );

#if defined( WV_CPP20 ) and defined( WV_PLATFORM_WINDOWS )
	template<typename = if_4x4::type>
#endif
	Vector4<Ty>& right( void ) { 
		return *reinterpret_cast<Vector4<Ty>*>( &m[ 0 * 4 ] ); 
	}

#if defined( WV_CPP20 ) and defined( WV_PLATFORM_WINDOWS )
	template<typename = if_4x4::type>
#endif
	Vector4<Ty>& up( void ) {
		return *reinterpret_cast<Vector4<Ty>*>( &m[ 1 * 4 ] );
	}

#if defined( WV_CPP20 ) and defined( WV_PLATFORM_WINDOWS )
	template<typename = if_4x4::type>
#endif
	Vector4<Ty>& at( void ) { 
		return *reinterpret_cast<Vector4<Ty>*>( &m[ 2 * 4 ] ); 
	}

#if defined( WV_CPP20 ) and defined( WV_PLATFORM_WINDOWS )
	template<typename = if_4x4::type>
#endif
	Vector4<Ty>& pos( void ) { 
		return *reinterpret_cast<Vector4<Ty>*>( &m[ 3 * 4 ] ); 
	}

	Matrix<Ty, Row, Col> inverse();
};

///////////////////////////////////////////////////////////////////////////////////////

namespace Math {

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty, size_t RowA, size_t ColARowB, size_t ColB>
Matrix<Ty, RowA, ColB> multiply( const Matrix<Ty, RowA, ColARowB>& _a, const Matrix<Ty, ColARowB, ColB>& _b )
{
	return MatrixMult<Ty, RowA, ColARowB, ColB>::multiply( _a, _b );
}

template<typename Ty>
static inline Matrix<Ty, 4, 4> inverse( const Matrix<Ty, 4, 4>& _m )
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

	Ty det = _m.get( 0, 0 ) * ( _m.get( 1, 1 ) * A2323 - _m.get( 1, 2 ) * A1323 + _m.get( 1, 3 ) * A1223 )
		- _m.get( 0, 1 ) * ( _m.get( 1, 0 ) * A2323 - _m.get( 1, 2 ) * A0323 + _m.get( 1, 3 ) * A0223 )
		+ _m.get( 0, 2 ) * ( _m.get( 1, 0 ) * A1323 - _m.get( 1, 1 ) * A0323 + _m.get( 1, 3 ) * A0123 )
		- _m.get( 0, 3 ) * ( _m.get( 1, 0 ) * A1223 - _m.get( 1, 1 ) * A0223 + _m.get( 1, 2 ) * A0123 );

	if( det == 0.0 ) // determinant is zero, inverse matrix does not exist
		return Matrix<Ty, 4, 4>{};

	det = 1 / det;

	Matrix<Ty, 4, 4> im;

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

template<typename Ty>
static inline Matrix<Ty, 3, 3> inverse( const Matrix<Ty, 3, 3>& _m )
{
	// computes the inverse of a matrix m
	Ty det = _m.get( 0, 0 ) * ( _m.get( 1, 1 ) * _m.get( 2, 2 ) - _m.get( 2, 1 ) * _m.get( 1, 2 ) ) -
		      _m.get( 0, 1 ) * ( _m.get( 1, 0 ) * _m.get( 2, 2 ) - _m.get( 1, 2 ) * _m.get( 2, 0 ) ) +
		      _m.get( 0, 2 ) * ( _m.get( 1, 0 ) * _m.get( 2, 1 ) - _m.get( 1, 1 ) * _m.get( 2, 0 ) );

	if ( det == 0.0 ) // determinant is zero, inverse matrix does not exist
		return Matrix<Ty, 3, 3>{};

	Ty invdet = 1 / det;

	Matrix<Ty, 3, 3> im{}; 

	im.set( 0, 0, _m.get( 1, 1 ) * _m.get( 2, 2 ) - _m.get( 2, 1 ) * _m.get( 1, 2 ) * invdet );
	im.set( 0, 1, _m.get( 0, 2 ) * _m.get( 2, 1 ) - _m.get( 0, 1 ) * _m.get( 2, 2 ) * invdet );
	im.set( 0, 2, _m.get( 0, 1 ) * _m.get( 1, 2 ) - _m.get( 0, 2 ) * _m.get( 1, 1 ) * invdet );
	im.set( 1, 0, _m.get( 1, 2 ) * _m.get( 2, 0 ) - _m.get( 1, 0 ) * _m.get( 2, 2 ) * invdet );
	im.set( 1, 1, _m.get( 0, 0 ) * _m.get( 2, 2 ) - _m.get( 0, 2 ) * _m.get( 2, 0 ) * invdet );
	im.set( 1, 2, _m.get( 1, 0 ) * _m.get( 0, 2 ) - _m.get( 0, 0 ) * _m.get( 1, 2 ) * invdet );
	im.set( 2, 0, _m.get( 1, 0 ) * _m.get( 2, 1 ) - _m.get( 2, 0 ) * _m.get( 1, 1 ) * invdet );
	im.set( 2, 1, _m.get( 2, 0 ) * _m.get( 0, 1 ) - _m.get( 0, 0 ) * _m.get( 2, 1 ) * invdet );
	im.set( 2, 2, _m.get( 0, 0 ) * _m.get( 1, 1 ) - _m.get( 1, 0 ) * _m.get( 0, 1 ) * invdet );

	return im;
}

template<typename Ty, size_t Row, size_t Col>
Matrix<Ty, Col, Row> transpose( const Matrix<Ty, Row, Col>& _m )
{
	Matrix<Ty, Col, Row> res;

	/// naive approach
	/// TODO: optimize
	for( size_t row = 0; row < Row; row++ )
		for( size_t col = 0; col < Col; col++ )
			res.set( col, row, _m.get( row, col ) );

	return res;
}

template<typename Ty>
Matrix<Ty, 4, 4> translate( const Matrix<Ty, 4, 4>& _m, const wv::Vector3<Ty>& _pos )
{
	Matrix<Ty, 4, 4> mat( Ty( 1 ) );

	mat.pos() = { _pos.x, _pos.y, _pos.z, Ty( 1 ) };

	return mat * _m;
}

template<typename Ty>
Matrix<Ty, 4, 4> scale( const Matrix<Ty, 4, 4>& _m, const wv::Vector3<Ty>& _scale )
{
	Matrix<Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, { _scale.x,      0.0,      0.0 } );
	mat.setRow( 1, { 0.0, _scale.y,      0.0 } );
	mat.setRow( 2, { 0.0,      0.0, _scale.z } );

	return mat * _m;
}

template<typename Ty>
Matrix<Ty, 4, 4> rotateX( const Matrix<Ty, 4, 4>& _m, Ty _radians )
{
	Matrix<Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, { 1.0,                0.0,                 0.0 } );
	mat.setRow( 1, { 0.0,  std::cos( _radians ), std::sin( _radians ) } );
	mat.setRow( 2, { 0.0, -std::sin( _radians ), std::cos( _radians ) } );

	return mat * _m;
}

template<typename Ty>
Matrix<Ty, 4, 4> rotateY( const Matrix<Ty, 4, 4>& _m, Ty _radians )
{
	Matrix<Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, { std::cos( _radians ), 0.0, -std::sin( _radians ) } );
	mat.setRow( 1, { 0.0, 1.0,                 0.0 } );
	mat.setRow( 2, { std::sin( _radians ), 0.0,  std::cos( _radians ) } );

	return mat * _m;
}

template<typename Ty>
Matrix<Ty, 4, 4> rotateZ( const Matrix<Ty, 4, 4>& _m, Ty _radians )
{
	Matrix<Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, {  std::cos( _radians ), std::sin( _radians ), 0.0 } );
	mat.setRow( 1, { -std::sin( _radians ), std::cos( _radians ), 0.0 } );
	mat.setRow( 2, { 0.0,                0.0, 1.0 } );

	return mat * _m;
}

template<typename Ty>
Matrix<Ty, 4, 4> perspective( const Ty& _aspect, const Ty& _fov, const Ty& _near, const Ty& _far )
{
	const Ty e = 1.0 / std::tan( _fov / 2.0 );
	const Ty m00 = e / _aspect;

#ifdef WV_GL_PRESPECTIVE_MATRIX
	const Ty m22 = ( _far + _near ) / ( _near - _far );
	const Ty m32 = ( 2 * _far * _near ) / ( _near - _far );

	Matrix<Ty, 4, 4> res{ {
		m00, 0,   0,  0,
		0,   e,   0,  0,
		0,   0, m22, -1,
		0,   0, m32,  0,
	} };
#else
	const Ty m22 = _near / ( _far - _near );
	const Ty m32 = _far * m22;

	Matrix<Ty, 4, 4> res{ {
		m00, 0,   0,    0,
		  0,   -e, 0,    0,
		  0,   0,   m22, -1,
		  0,   0,   m32,  0,
	} };
#endif

	return res;
}

template<typename Ty>
Matrix<Ty, 4, 4> orthographic( const Ty& _halfWidth, const Ty& _halfHeight, const Ty& _far, const Ty& _near )
{
	Ty m00 = 1.0 / _halfWidth;
	Ty m11 = 1.0 / _halfHeight;
	Ty m22 = ( -2.0 ) / ( _far - _near );
	Ty m32 = -( ( _far + _near ) / ( _far - _near ) );

	Matrix<Ty, 4, 4> res{ {
		m00,   0,   0, 0,
		0,   m11,   0, 0,
		0,     0, m22, 0,
		0,     0, m32, 1,
	} };

	return res;
}

template<typename Ty>
Matrix<Ty, 4, 4> focalPerspective( Ty _sensorWidth, Ty _sensorHeight, Ty _focalLength, Ty _near, Ty _far )
{
	Ty fov = 2.0 * atan( _sensorWidth / ( 2.0 * _focalLength ) );
	return perspective<Ty>( _sensorHeight / _sensorWidth, fov, _near, _far );
}

}

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
Vector4<Ty> operator*( const Matrix<Ty, 4, 4>& _mat, const Vector4<Ty>& _vec )
{
	Matrix<Ty, 4, 1> tmpMat{};
	tmpMat.set( 0, 0, _vec.x );
	tmpMat.set( 1, 0, _vec.y );
	tmpMat.set( 2, 0, _vec.z );
	tmpMat.set( 3, 0, _vec.w );

	const Matrix<Ty, 4, 1> res = _mat * tmpMat;

	return {
		res.get( 0, 0 ),
		res.get( 1, 0 ),
		res.get( 2, 0 ),
		res.get( 3, 0 )
	};
}

template<typename Ty>
Vector4<Ty> operator*( const Vector4<Ty>& _vec, const Matrix<Ty, 4, 4>& _mat )
{
	const Matrix<Ty, 1, 4> res = Matrix<Ty, 1, 4>{ _vec } * _mat;

	return {
		res.get( 0, 0 ),
		res.get( 0, 1 ),
		res.get( 0, 2 ),
		res.get( 0, 3 )
	};
}

template<typename Ty>
Vector3<Ty> operator*( const Matrix<Ty, 3, 3>& _mat, const Vector3<Ty>& _vec )
{
	Matrix<Ty, 3, 1> tmpMat{};
	tmpMat.set( 0, 0, _vec.x );
	tmpMat.set( 1, 0, _vec.y );
	tmpMat.set( 2, 0, _vec.z );
	
	const Matrix<Ty, 3, 1> res = _mat * tmpMat;

	return {
		res.get( 0, 0 ),
		res.get( 1, 0 ),
		res.get( 2, 0 )
	};
}

template<typename Ty>
Vector3<Ty> operator*( const Vector3<Ty>& _vec, const Matrix<Ty, 3, 3>& _mat )
{
	const Matrix<Ty, 1, 3> res = Matrix<Ty, 1, 3>{ _vec } * _mat;

	return {
		res.get( 0, 0 ),
		res.get( 0, 1 ),
		res.get( 0, 2 )
	};
}

template<typename Ty>
Vector3<Ty> operator*( const Matrix<Ty, 4, 4>& _mat, const Vector3<Ty>& _vec )
{
	Vector4f vec4{ _vec.x, _vec.y, _vec.z, 1.0f };
	Vector4f ret4 = _mat * vec4;
	return { ret4.x, ret4.y, ret4.z };
}

template<typename Ty>
Vector3<Ty> operator*( const Vector3<Ty>& _vec, const Matrix<Ty, 4, 4>& _mat )
{
	Vector4f vec4{ _vec.x, _vec.y, _vec.z, 1.0f };
	Vector4f ret4 = vec4 * _mat;
	return { ret4.x, ret4.y, ret4.z };
}

///////////////////////////////////////////////////////////////////////////////////////

typedef Matrix<float, 4, 4> Matrix4x4f;
typedef Matrix<float, 3, 3> Matrix3x3f;

///////////////////////////////////////////////////////////////////////////////////////

template <typename Ty, size_t Row, size_t Col>
inline Matrix<Ty, Row, Col>& Matrix<Ty, Row, Col>::operator=( const Matrix<Ty, Row, Col>& _o )
{
	static_assert( sizeof( m ) == sizeof( _o.m ), "Invalid matrix::m size" );
	std::memcpy( &m, &_o.m, sizeof( m ) );
	return *this;
}

template<typename Ty, size_t Row, size_t Col>
template<size_t ColB>
inline Matrix<Ty, Row, ColB> Matrix<Ty, Row, Col>::operator*( const Matrix<Ty, Col, ColB>& _o ) const
{
	return Math::multiply( *this, _o );
}

template<typename Ty, size_t Row, size_t Col>
template<size_t ColB>
inline Matrix<Ty, Row, Col>& Matrix<Ty, Row, Col>::operator*=( const Matrix<Ty, Row, ColB>& _o )
{
	*this = (*this) * _o;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty, size_t Row, size_t Col>
inline void Matrix<Ty, Row, Col>::setRow( const size_t& _r, std::array<Ty, Col> _v )
{
#ifdef __cpp_exceptions
	if( _r >= Row )
		throw std::runtime_error( "out of range row or column" );
#endif
	size_t idx = 0;
	for( auto& v : _v )
	{
		set( _r, idx, v );
		idx++;
	}
}

template<typename Ty, size_t Row, size_t Col>
inline Matrix<Ty, Row, Col> Matrix<Ty, Row, Col>::inverse()
{
	return Math::inverse<Ty>( *this );
}

}