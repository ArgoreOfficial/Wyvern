#pragma once

#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <stdint.h>
#include <type_traits>
#include <array>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {
template<typename _Ty, size_t _RowA, size_t _ColA_RowB, size_t _ColB>
struct MatrixMult;

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty, size_t _Row, size_t _Col>
class Matrix
{
public:

	typedef std::enable_if<_Row == 4 && _Col == 4> if_4x4;
	typedef std::enable_if<_Row == 3 && _Col == 3> if_3x3;
	typedef std::enable_if<_Row == _Col> if_square;

///////////////////////////////////////////////////////////////////////////////////////

	_Ty m[ _Row * _Col ];

///////////////////////////////////////////////////////////////////////////////////////

	Matrix( void ) : m{ 0 } {}
	Matrix( const _Ty& _val ) :
		m{ 0 }
	{
		static_assert( _Row == _Col, "Cannot create identity matrix from non-square matrix. See output" );
		for( size_t i = 0; i < _Row; i++ )
			set( i, i, _val );
	}

	Matrix( const Matrix<_Ty, _Row, _Col>& _other )
	{
		std::memcpy( m, _other.m, sizeof( m ) );
	}

	Matrix( const _Ty( &_list )[ _Row * _Col ] )
	{
		std::memcpy( m, _list, sizeof( m ) );
	}

///////////////////////////////////////////////////////////////////////////////////////

	_Ty* operator []( const size_t& _index ) const { 
		return (_Ty*)m[ _index ]; 
	}

	                       Matrix<_Ty, _Row, _Col>& operator = ( const Matrix<_Ty, _Row, _Col>&  _o );
	template<size_t _ColB> Matrix<_Ty, _Row, _ColB> operator * ( const Matrix<_Ty, _Col, _ColB>& _o ) const;
	template<size_t _ColB> Matrix<_Ty, _Row, _Col>& operator *=( const Matrix<_Ty, _Row, _ColB>& _o );

///////////////////////////////////////////////////////////////////////////////////////

	static Matrix<_Ty, _Row, _Row> identity( const _Ty& _val )
	{
		return Matrix<_Ty, _Row, _Row>( _val );
	}

///////////////////////////////////////////////////////////////////////////////////////

	inline void set( size_t _r, size_t _c, _Ty _val ) {
	#ifdef __cpp_exceptions
		if( _r >= _Row || _c >= _Col )
			throw std::runtime_error( "out of range row or column" );
	#endif
		m[ _r * _Col + _c ] = _val;
	}

	inline _Ty get( size_t _r, size_t _c ) const {
	#ifdef __cpp_exceptions
		if( _r >= _Row || _c >= _Col )
			throw std::runtime_error( "out of range row or column" );
	#endif
		return m[ _r * _Col + _c ];
	}

	void setRow( const size_t& _r, std::array<_Ty, _Col> _v );

#ifdef WV_CPP20
	template<typename = if_4x4::type>
#endif
	Vector4<_Ty>& right( void ) { 
		return *reinterpret_cast<Vector4<_Ty>*>( &m[ 0 * 4 ] ); 
	}

#ifdef WV_CPP20
	template<typename = if_4x4::type>
#endif
	Vector4<_Ty>& up( void ) {
		return *reinterpret_cast<Vector4<_Ty>*>( &m[ 1 * 4 ] );
	}

#ifdef WV_CPP20
	template<typename = if_4x4::type>
#endif
	Vector4<_Ty>& at( void ) { 
		return *reinterpret_cast<Vector4<_Ty>*>( &m[ 2 * 4 ] ); 
	}

#ifdef WV_CPP20
	template<typename = if_4x4::type>
#endif
	Vector4<_Ty>& pos( void ) { 
		return *reinterpret_cast<Vector4<_Ty>*>( &m[ 3 * 4 ] ); 
	}

	Matrix<_Ty, _Row, _Col> inverse();
};

///////////////////////////////////////////////////////////////////////////////////////

namespace Math {

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty, size_t _RowA, size_t _ColA_RowB, size_t _ColB>
Matrix<_Ty, _RowA, _ColB> multiply( const Matrix<_Ty, _RowA, _ColA_RowB>& _a, const Matrix<_Ty, _ColA_RowB, _ColB>& _b )
{
	return MatrixMult<_Ty, _RowA, _ColA_RowB, _ColB>::multiply( _a, _b );
}

template<typename _Ty>
static inline Matrix<_Ty, 4, 4> inverse( const Matrix<_Ty, 4, 4>& _m )
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

	_Ty det = _m.get( 0, 0 ) * ( _m.get( 1, 1 ) * A2323 - _m.get( 1, 2 ) * A1323 + _m.get( 1, 3 ) * A1223 )
		- _m.get( 0, 1 ) * ( _m.get( 1, 0 ) * A2323 - _m.get( 1, 2 ) * A0323 + _m.get( 1, 3 ) * A0223 )
		+ _m.get( 0, 2 ) * ( _m.get( 1, 0 ) * A1323 - _m.get( 1, 1 ) * A0323 + _m.get( 1, 3 ) * A0123 )
		- _m.get( 0, 3 ) * ( _m.get( 1, 0 ) * A1223 - _m.get( 1, 1 ) * A0223 + _m.get( 1, 2 ) * A0123 );

	if( det == 0.0 ) // determinant is zero, inverse matrix does not exist
		return Matrix<_Ty, 4, 4>{};

	det = 1 / det;

	Matrix<_Ty, 4, 4> im;

	im.set( 0, 0, det * ( _m.get( 1, 1 ) * A2323 - _m.get( 1, 2 ) * A1323 + _m.get( 1, 3 ) * A1223 ) );
	im.set( 0, 1, det * -( _m.get( 0, 1 ) * A2323 - _m.get( 0, 2 ) * A1323 + _m.get( 0, 3 ) * A1223 ) );
	im.set( 0, 2, det * ( _m.get( 0, 1 ) * A2313 - _m.get( 0, 2 ) * A1313 + _m.get( 0, 3 ) * A1213 ) );
	im.set( 0, 3, det * -( _m.get( 0, 1 ) * A2312 - _m.get( 0, 2 ) * A1312 + _m.get( 0, 3 ) * A1212 ) );
	im.set( 1, 0, det * -( _m.get( 1, 0 ) * A2323 - _m.get( 1, 2 ) * A0323 + _m.get( 1, 3 ) * A0223 ) );
	im.set( 1, 1, det * ( _m.get( 0, 0 ) * A2323 - _m.get( 0, 2 ) * A0323 + _m.get( 0, 3 ) * A0223 ) );
	im.set( 1, 2, det * -( _m.get( 0, 0 ) * A2313 - _m.get( 0, 2 ) * A0313 + _m.get( 0, 3 ) * A0213 ) );
	im.set( 1, 3, det * ( _m.get( 0, 0 ) * A2312 - _m.get( 0, 2 ) * A0312 + _m.get( 0, 3 ) * A0212 ) );
	im.set( 2, 0, det * ( _m.get( 1, 0 ) * A1323 - _m.get( 1, 1 ) * A0323 + _m.get( 1, 3 ) * A0123 ) );
	im.set( 2, 1, det * -( _m.get( 0, 0 ) * A1323 - _m.get( 0, 1 ) * A0323 + _m.get( 0, 3 ) * A0123 ) );
	im.set( 2, 2, det * ( _m.get( 0, 0 ) * A1313 - _m.get( 0, 1 ) * A0313 + _m.get( 0, 3 ) * A0113 ) );
	im.set( 2, 3, det * -( _m.get( 0, 0 ) * A1312 - _m.get( 0, 1 ) * A0312 + _m.get( 0, 3 ) * A0112 ) );
	im.set( 3, 0, det * -( _m.get( 1, 0 ) * A1223 - _m.get( 1, 1 ) * A0223 + _m.get( 1, 2 ) * A0123 ) );
	im.set( 3, 1, det * ( _m.get( 0, 0 ) * A1223 - _m.get( 0, 1 ) * A0223 + _m.get( 0, 2 ) * A0123 ) );
	im.set( 3, 2, det * -( _m.get( 0, 0 ) * A1213 - _m.get( 0, 1 ) * A0213 + _m.get( 0, 2 ) * A0113 ) );
	im.set( 3, 3, det * ( _m.get( 0, 0 ) * A1212 - _m.get( 0, 1 ) * A0212 + _m.get( 0, 2 ) * A0112 ) );

	return im;
}

template<typename _Ty, size_t _Row, size_t _Col>
Matrix<_Ty, _Col, _Row> transpose( const Matrix<_Ty, _Row, _Col>& _m )
{
	Matrix<_Ty, _Col, _Row> res;

	/// naive approach
	/// TODO: optimize
	for( size_t row = 0; row < _Row; row++ )
		for( size_t col = 0; col < _Col; col++ )
			res.set( col, row, _m.get( row, col ) );

	return res;
}

template<typename _Ty>
Matrix<_Ty, 4, 4> translate( const Matrix<_Ty, 4, 4>& _m, const wv::Vector3<_Ty>& _pos )
{
	Matrix<_Ty, 4, 4> mat( _Ty( 1 ) );

	mat.pos() = { _pos.x, _pos.y, _pos.z, _Ty( 1 ) };

	return mat * _m;
}

template<typename _Ty>
Matrix<_Ty, 4, 4> scale( const Matrix<_Ty, 4, 4>& _m, const wv::Vector3<_Ty>& _scale )
{
	Matrix<_Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, { _scale.x,      0.0,      0.0 } );
	mat.setRow( 1, { 0.0, _scale.y,      0.0 } );
	mat.setRow( 2, { 0.0,      0.0, _scale.z } );

	return mat * _m;
}

template<typename _Ty>
Matrix<_Ty, 4, 4> rotateX( const Matrix<_Ty, 4, 4>& _m, _Ty _angle )
{
	Matrix<_Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, { 1.0,                0.0,                 0.0 } );
	mat.setRow( 1, { 0.0,  std::cos( _angle ), std::sin( _angle ) } );
	mat.setRow( 2, { 0.0, -std::sin( _angle ), std::cos( _angle ) } );

	return mat * _m;
}

template<typename _Ty>
Matrix<_Ty, 4, 4> rotateY( const Matrix<_Ty, 4, 4>& _m, _Ty _angle )
{
	Matrix<_Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, { std::cos( _angle ), 0.0, -std::sin( _angle ) } );
	mat.setRow( 1, { 0.0, 1.0,                 0.0 } );
	mat.setRow( 2, { std::sin( _angle ), 0.0,  std::cos( _angle ) } );

	return mat * _m;
}

template<typename _Ty>
Matrix<_Ty, 4, 4> rotateZ( const Matrix<_Ty, 4, 4>& _m, _Ty _angle )
{
	Matrix<_Ty, 4, 4> mat( 1.0 );

	mat.setRow( 0, { std::cos( _angle ), std::sin( _angle ), 0.0 } );
	mat.setRow( 1, { -std::sin( _angle ), std::cos( _angle ), 0.0 } );
	mat.setRow( 2, { 0.0,                0.0, 1.0 } );

	return mat * _m;
}

// https://jsantell.com/3d-projection/#field-of-view
template<typename _Ty>
Matrix<_Ty, 4, 4> perspective( const _Ty& _aspect, const _Ty& _fov, const _Ty& _near, const _Ty& _far )
{
	const _Ty e = 1.0 / std::tan( _fov / 2.0 );
	const _Ty m00 = e / _aspect;
	const _Ty m22 = ( _far + _near ) / ( _near - _far );
	const _Ty m32 = ( 2 * _far * _near ) / ( _near - _far );

	Matrix<_Ty, 4, 4> res{ {
		m00, 0,   0,  0,
		0,   e,   0,  0,
		0,   0, m22, -1,
		0,   0, m32,  0,
	} };

	return res;
}

template<typename _Ty>
Matrix<_Ty, 4, 4> orthographic( const _Ty& _halfWidth, const _Ty& _halfHeight, const _Ty& _far, const _Ty& _near )
{
	_Ty m00 = 1.0 / _halfWidth;
	_Ty m11 = 1.0 / _halfHeight;
	_Ty m22 = ( -2.0 ) / ( _far - _near );
	_Ty m32 = -( ( _far + _near ) / ( _far - _near ) );

	Matrix<_Ty, 4, 4> res{ {
		m00,   0,   0, 0,
		0,   m11,   0, 0,
		0,     0, m22, 0,
		0,     0, m32, 1,
	} };

	return res;
}

/// TODO: focal length camera https://paulbourke.net/miscellaneous/lens/

template<typename _Ty>
Matrix<_Ty, 1, 4> fromVector( const Vector4<_Ty>& _vec )
{
	Matrix<_Ty, 1, 4> m;
	m.setRow( 0, { _vec.x, _vec.y, _vec.z, _vec.w } );
	return m;
}

template<typename _Ty>
Matrix<_Ty, 1, 3> fromVector( Vector3<_Ty> _vec )
{
	Matrix<_Ty, 1, 3> m;
	m.setRow( 0, { _vec.x, _vec.y, _vec.z } );
	return m;
}

}

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty>
Vector4<_Ty> operator*( const Matrix<_Ty, 4, 4>& _mat, const Vector4<_Ty>& _vec )
{
	Matrix<_Ty, 4, 1> tmpMat{};
	tmpMat.set( 0, 0, _vec.x );
	tmpMat.set( 1, 0, _vec.y );
	tmpMat.set( 2, 0, _vec.z );
	tmpMat.set( 3, 0, _vec.w );

	const Matrix<_Ty, 4, 1> res = _mat * tmpMat;

	return {
		res.get( 0, 0 ),
		res.get( 1, 0 ),
		res.get( 2, 0 ),
		res.get( 3, 0 )
	};
}

template<typename _Ty>
Vector4<_Ty> operator*( const Vector4<_Ty>& _vec, const Matrix<_Ty, 4, 4>& _mat )
{
	Matrix<_Ty, 1, 4> tmpMat{};
	tmpMat.setRow( 0, { _vec.x, _vec.y, _vec.z, _vec.w } );

	const Matrix<_Ty, 1, 4> res = tmpMat * _mat;

	return {
		res.get( 0, 0 ),
		res.get( 0, 1 ),
		res.get( 0, 2 ),
		res.get( 0, 3 )
	};
}

template<typename _Ty>
Vector3<_Ty> operator*( const Matrix<_Ty, 4, 4>& _mat, const Vector3<_Ty>& _vec )
{
	Vector4f vec4{ _vec.x, _vec.y, _vec.z, 1.0f };
	Vector4f ret4 = _mat * vec4;
	return { ret4.x, ret4.y, ret4.z };
}

template<typename _Ty>
Vector3<_Ty> operator*( const Vector3<_Ty>& _vec, const Matrix<_Ty, 4, 4>& _mat )
{
	Vector4f vec4{ _vec.x, _vec.y, _vec.z, 1.0f };
	Vector4f ret4 = vec4 * _mat;
	return { ret4.x, ret4.y, ret4.z };
}

///////////////////////////////////////////////////////////////////////////////////////

typedef Matrix<float, 4, 4> Matrix4x4f;
typedef Matrix<float, 3, 3> Matrix3x3f;

///////////////////////////////////////////////////////////////////////////////////////

template <typename _Ty, size_t _Row, size_t _Col>
inline Matrix<_Ty, _Row, _Col>& Matrix<_Ty, _Row, _Col>::operator=( const Matrix<_Ty, _Row, _Col>& _o )
{
	static_assert( sizeof( m ) == sizeof( _o.m ), "Invalid matrix::m size" );
	std::memcpy( &m, &_o.m, sizeof( m ) );
	return *this;
}

template<typename _Ty, size_t _Row, size_t _Col>
template<size_t _ColB>
inline Matrix<_Ty, _Row, _ColB> Matrix<_Ty, _Row, _Col>::operator*( const Matrix<_Ty, _Col, _ColB>& _o ) const
{
	return Math::multiply( *this, _o );
}

template<typename _Ty, size_t _Row, size_t _Col>
template<size_t _ColB>
inline Matrix<_Ty, _Row, _Col>& Matrix<_Ty, _Row, _Col>::operator*=( const Matrix<_Ty, _Row, _ColB>& _o )
{
	*this = (*this) * _o;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty, size_t _Row, size_t _Col>
inline void Matrix<_Ty, _Row, _Col>::setRow( const size_t& _r, std::array<_Ty, _Col> _v )
{
#ifdef __cpp_exceptions
	if( _r >= _Row )
		throw std::runtime_error( "out of range row or column" );
#endif
	size_t idx = 0;
	for( auto& v : _v )
	{
		set( _r, idx, v );
		idx++;
	}
}

template<typename _Ty, size_t _Row, size_t _Col>
inline Matrix<_Ty, _Row, _Col> Matrix<_Ty, _Row, _Col>::inverse()
{
	return Math::inverse<_Ty>( *this );
}

}