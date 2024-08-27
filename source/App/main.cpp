
#include "Sandbox.h"

///////////////////////////////////////////////////////////////////////////////////////
#include <wv/Math/Matrix.h>
#include <wv/Math/Quaternion.h>
int main()
{
	cSandbox sandbox{};

	wv::cQuaternionf quat = wv::cQuaternionf::fromAxisAngle( { 1.0f, 0.0f, 0.0f }, wv::Math::degToRad( 10.0f ) );

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

	if ( sandbox.create() )
	{
		sandbox.run();
		sandbox.destroy();
	}

	return 0;
}