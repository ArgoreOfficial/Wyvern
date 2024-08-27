
#include "Sandbox.h"

///////////////////////////////////////////////////////////////////////////////////////
#include <wv/Math/Matrix.h>

int main()
{
	cSandbox sandbox{};
	/*
	wv::cMatrix<float, 2, 3> matA;
	matA.setRow( 0, { 1, 4, -2 } );
	matA.setRow( 0, { 3, 5, -6 } );

	wv::cMatrix<float, 3, 4> matB;
	matB.setRow( 0, {  5, 2, 8, -1 } );
	matB.setRow( 1, {  3, 6, 4,  5 } );
	matB.setRow( 2, { -2, 9, 7, -3 } );
	
	auto newMat = matA * matB;

	wv::cMatrix4x4f mat4x4( 5.0f );
	mat4x4 = wv::cMatrix4x4f::identity( 5.0f );
	
	mat4x4.right() = { 1.0f, 2.0f, 3.0f, 4.0f };
	*/




	wv::cMatrix<float, 3, 3> A;
	A.setRow( 0, { 1.f, 2.f, 3.f } );
	A.setRow( 1, { 4.f, 5.f, 6.f } );
	A.setRow( 2, { 7.f, 8.f, 9.f } );

	wv::cMatrix<float, 3, 3> G;
	G.setRow( 0, { -5.f/3.f,  2.f/3.f, 0.f } );
	G.setRow( 1, {  4.f/3.f, -1.f/3.f, 0.f } );
	G.setRow( 2, {      0.f,      0.f, 0.f } );

	auto I = A * G * A;

	wv::cVector3f vec{ 3.f, 4.f, 2.f };
	wv::cMatrix4x4f trl = wv::Matrix::translation( vec );
	wv::cMatrix4x4f scl = wv::Matrix::scalar( vec );

	if ( sandbox.create() )
	{
		sandbox.run();
		sandbox.destroy();
	}

	return 0;
}