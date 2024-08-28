
#include "Sandbox.h"

///////////////////////////////////////////////////////////////////////////////////////
#include <wv/Math/Matrix.h>
#include <wv/Math/Quaternion.h>

int main()
{
	cSandbox sandbox{};

	/*
	wv::cMatrix<float, 3, 3> A;
	A.setRow( 0, { 1.f, 2.f, 3.f } );
	A.setRow( 1, { 4.f, 5.f, 6.f } );
	A.setRow( 2, { 7.f, 8.f, 9.f } );

	wv::cMatrix<float, 3, 3> G;
	G.setRow( 0, { -5.f/3.f,  2.f/3.f, 0.f } );
	G.setRow( 1, {  4.f/3.f, -1.f/3.f, 0.f } );
	G.setRow( 2, {      0.f,      0.f, 0.f } );

	auto I = A * G * A;
	*/

	wv::cMatrix<float, 3, 2> tsp;
	tsp.setRow( 0, { 1,2 } );
	tsp.setRow( 1, { 3,4 } );
	tsp.setRow( 2, { 5,6 } );

	auto ntsp = wv::Matrix::transpose( tsp );

	wv::cMatrix4x4f test;
	test.setRow( 0, { 3, 2, 0, 0 } );
	test.setRow( 1, { 4, 3, 0, 0 } );
	test.setRow( 2, { 0, 0, 6, 5 } );
	test.setRow( 3, { 0, 0, 7, 6 } );

	wv::cMatrix4x4f inv = wv::Matrix::inverse( test );
	auto id = test * inv; // identity

	if ( sandbox.create() )
	{
		sandbox.run();
		sandbox.destroy();
	}

	return 0;
}