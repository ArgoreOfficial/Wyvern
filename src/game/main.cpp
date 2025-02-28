
#include "Sandbox.h"

#include <wv/debug/trace.h>
#include <wv/memory/memory.h>
#include <wv/console/console.h>

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

int main( int argc, char* argv[] )
{
	wv::Trace::Trace::printEnabled = false;

	if( !wv::Console::isInitialized() )
		wv::Console::initialize();

	/// TODO: move to platform specific, might not always have access to argv
	if( strncmp( argv[ 0 ], "RMT", 3 ) == 0 ) // is remote client
	{
		wv::Debug::Print( "Launching Remote Client" );
		std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
	}

	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Initializing Application Configuration\n" );

	Sandbox sandbox{};

#ifdef WV_PACKAGE
#ifdef __cpp_exceptions
	try
	{
#endif
#endif
		if( sandbox.create() )
		{
			wv::Debug::Print( "Starting Run Loop\n" );
			sandbox.run();
			wv::Debug::Print( "Ending Run Loop\n" );
			sandbox.destroy();
		}
#ifdef WV_PACKAGE
#ifdef __cpp_exceptions
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
#endif
	
	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Program Exit\n" );
	
	wv::MemoryTracker::dump();

#ifdef WV_PLATFORM_3DS
	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
#endif

	if ( wv::Console::isInitialized() )
		wv::Console::deinitialize();

	return 0;
}