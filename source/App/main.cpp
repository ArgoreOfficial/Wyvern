
#include "Sandbox.h"

#include <wv/Defines.h>
#include <wv/Debug/Trace.h>

#include <stdio.h>

#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
WV_PSVITA_THREAD( "Sandbox" );
WV_PSVITA_HEAPSIZE( 1 * 1024 * 1024 );
#endif

///////////////////////////////////////////////////////////////////////////////////////

int main()
{
	wv::Trace::sTrace::printEnabled = false;

	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Initializing Application Configuration\n" );

	cSandbox sandbox{};

	if( sandbox.create() )
	{
		wv::Debug::Print( "Starting Run Loop\n" );
		sandbox.run();
		wv::Debug::Print( "Ending Run Loop\n" );
		sandbox.destroy();
	}
	
	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Program Exit\n" );
	
	return 0;
}