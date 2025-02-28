#pragma once

#include "matrix_core.h"

#ifdef WV_ENABLE_SIMD
#include <xmmintrin.h>
#endif

namespace wv {

template<typename T, size_t RowA, size_t ColA_RowB, size_t ColB>
struct MatrixMult
{
	static Matrix<T, RowA, ColB> multiply( const Matrix<T, RowA, ColA_RowB>& _a, const Matrix<T, ColA_RowB, ColB>& _b )
	{
		Matrix<T, RowA, ColB> res;
		for( size_t row = 0; row < RowA; row++ )
		{
			for( size_t column = 0; column < ColB; column++ )
			{
				T v = 0;

				for( size_t inner = 0; inner < ColA_RowB; inner++ )
					v += _a.get( row, inner ) * _b.get( inner, column );

				res.set( row, column, v );
			}
		}
		return res;
	}
};

#ifdef WV_ENABLE_SIMD
template<typename _Ty>
struct MatrixMult<_Ty, 4, 4, 4>
{
	static Matrix<_Ty, 4, 4> multiply( const Matrix<_Ty, 4, 4>& _a, const Matrix<_Ty, 4, 4>& _b )
	{
		Matrix<_Ty, 4, 4> out;
		__m128 row1 = _mm_load_ps( &_b.m[ 0 ] );
		__m128 row2 = _mm_load_ps( &_b.m[ 4 ] );
		__m128 row3 = _mm_load_ps( &_b.m[ 8 ] );
		__m128 row4 = _mm_load_ps( &_b.m[ 12 ] );
		for( int i = 0; i < 4; i++ )
		{
			__m128 brod1 = _mm_set1_ps( _a.m[ 4 * i + 0 ] );
			__m128 brod2 = _mm_set1_ps( _a.m[ 4 * i + 1 ] );
			__m128 brod3 = _mm_set1_ps( _a.m[ 4 * i + 2 ] );
			__m128 brod4 = _mm_set1_ps( _a.m[ 4 * i + 3 ] );
			__m128 row = _mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps( brod1, row1 ),
					_mm_mul_ps( brod2, row2 ) ),
				_mm_add_ps(
					_mm_mul_ps( brod3, row3 ),
					_mm_mul_ps( brod4, row4 ) ) );
			_mm_store_ps( &out.m[ 4 * i ], row );
		}
		return out;
	}
};
#endif

}
