
#include "Sandbox.h"

#include <wv/Debug/Trace.h>
#include <wv/Memory/Memory.h>

#include <stdio.h>
#include <exception>
#include <typeinfo>
#include <stdexcept>

#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
WV_PSVITA_THREAD( "Sandbox" );
WV_PSVITA_HEAPSIZE( 1 * 1024 * 1024 );
#endif

#include <wv/Console/Console.h>

///////////////////////////////////////////////////////////////////////////////////////

int main()
{
	wv::Trace::sTrace::printEnabled = false;

	if( !wv::Console::isInitialized() )
		wv::Console::initialize();

	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Initializing Application Configuration\n" );

	cSandbox sandbox{};

#ifdef WV_PACKAGE
	try
	{
#endif
		if( sandbox.create() )
		{
			wv::Debug::Print( "Starting Run Loop\n" );
			sandbox.run();
			wv::Debug::Print( "Ending Run Loop\n" );
			sandbox.destroy();
		}
#ifdef WV_PACKAGE
	}
	catch( const std::runtime_error& re )
	{
		printf( "\n[========= Runtime Error Occured ==========]\n"
				"\n%s\n"
				"\n[==========================================]\n", re.what() );
	}
	catch( const std::exception& ex )
	{
		printf( "\n[=========== Exception Occured ============]\n"
				"\n%s\n"
				"\n[==========================================]\n", ex.what() );
	}
	catch( ... )
	{
		printf( "\n[============ Unknown Failure =============]\n"
				"\nPossible memory corruption\n\n"
				"\n[==========================================]\n" );
	}
#endif
	
	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Program Exit\n" );
	
#ifdef WV_PLATFORM_3DS
	while( true ) {}
#endif

	return 0;
}