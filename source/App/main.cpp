
#include "Sandbox.h"

///////////////////////////////////////////////////////////////////////////////////////

int main()
{
	cSandbox sandbox{};

	if ( sandbox.create() )
	{
		sandbox.run();
		sandbox.destroy();
	}

	return 0;
}