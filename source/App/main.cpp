
#include "Sandbox.h"

///////////////////////////////////////////////////////////////////////////////////////
#include <wv/Math/Matrix.h>

int main()
{
	cSandbox sandbox{};

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

	if ( sandbox.create() )
	{
		sandbox.run();
		sandbox.destroy();
	}

	return 0;
}