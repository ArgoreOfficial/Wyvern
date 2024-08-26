#pragma once

namespace wv
{
	namespace Matrix
	{
		template<typename T, unsigned int R, unsigned int C>
		struct sPureMatrix
		{
			sPureMatrix() : m{ 0 } { }
			T m[ R ][ C ];

			T* operator []( unsigned int _index ) const { return (T*)m[ _index ]; }

			void setRow( unsigned int _r, T _v[ C ] )
			{
				for ( int i = 0; i < C; i++ )
					m[ _r ][ i ] = _v[ i ];
			}
		};

		template<typename T, unsigned int S>
		static sPureMatrix<T, S, S> identity( T _val )
		{
			sPureMatrix<T, S, S> mat{};
			for ( unsigned int i = 0; i < S; i++ )
				mat[ i ][ i ] = _val;
			return mat;
		}

		template<typename T, unsigned int RowA, unsigned int ColA_RowB, unsigned int ColB>
		sPureMatrix<T, RowA, ColB> pureMult( sPureMatrix<T, RowA, ColA_RowB> _a, sPureMatrix<T, ColA_RowB, ColB> _b )
		{
			sPureMatrix<T, RowA, ColB> res;
			for ( unsigned int row = 0; row < RowA; row++ )
			{
				for ( unsigned int column = 0; column < ColB; column++ )
				{
					T v = 0;
					printf( "[" );
					for ( int inner = 0; inner < ColA_RowB; inner++ )
					{
						v += _a.m[ row ][ inner ] * _b.m[ inner ][ column ];
					}
					res[ row ][ column ] = v;
					printf( "%.0f", v );
					printf( "] " );
				}
				printf( "\n" );
			}
			return res;
		}
	}
}