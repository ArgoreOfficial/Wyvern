
#include "Sandbox.h"

///////////////////////////////////////////////////////////////////////////////////////
#include <wv/Math/Matrix4x4.h>

int main()
{
	cSandbox sandbox{};

	float A[ 2 ][ 3 ] = {
		{ 1, 4, -2 },
		{ 3, 5, -6 }
	};

	float B[ 3 ][ 4 ] = {
		{  5, 2, 8, -1 },
		{  3, 6, 4,  5 },
		{ -2, 9, 7, -3 }
	};

	wv::Matrix::sPureMatrix<float, 2, 3> matA;
	matA.setRow( 0, A[ 0 ] );
	matA.setRow( 1, A[ 1 ] );

	wv::Matrix::sPureMatrix<float, 3, 4> matB;
	matB.setRow( 0, B[ 0 ] );
	matB.setRow( 1, B[ 1 ] );
	matB.setRow( 2, B[ 2 ] );

	wv::Matrix::sPureMatrix<float, 4, 4> mat3 = wv::Matrix::identity<float, 4>( 1.0f );

	wv::cMatrix4x4<float> mat4x4f{ 1.0f };
	mat4x4f.right() = wv::cVector4<float>{ 3.0f, 1.0f, 2.0f, 4.0f };
	wv::cVector4<float> right = mat4x4f.right();

	wv::Matrix::pureMult( matA, matB );

	if ( sandbox.create() )
	{
		sandbox.run();
		sandbox.destroy();
	}

	return 0;
}