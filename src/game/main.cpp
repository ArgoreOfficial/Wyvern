
#include "Sandbox.h"

#include <wv/debug/trace.h>
#include <wv/memory/memory.h>
#include <wv/platform/console.h>

#include <stdio.h>
#include <exception>
#include <typeinfo>
#include <stdexcept>

#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
WV_PSVITA_THREAD( "Sandbox" );
WV_PSVITA_HEAPSIZE( 1 * 1024 * 1024 );
#endif

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
	
	wv::MemoryTracker::dump();

#ifdef WV_PLATFORM_3DS
	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Closing in 5 seconds...\n" );
	std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
#endif

	return 0;
}