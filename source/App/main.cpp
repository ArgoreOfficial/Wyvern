
#include "Sandbox.h"

///////////////////////////////////////////////////////////////////////////////////////

int main()
{
	cSandbox* sandbox = new cSandbox();

	if ( sandbox->create() )
	{
		sandbox->run();
		sandbox->destroy();
	}

	return 0;
}