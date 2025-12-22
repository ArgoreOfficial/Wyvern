#include "wv/application.h"

#include <wv/console/console.h>
#include <wv/debug/error.h>
#include <wv/debug/trace.h>
#include <wv/memory/memory.h>
#include <wv/target/iremote_target.h>
#include <wv/entity/world.h>

#include <wv/platform/platform.h>
#include <wv/platform/thread.h>

#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
WV_PSVITA_THREAD( "Sandbox" );
WV_PSVITA_HEAPSIZE( 1 * 1024 * 1024 );
#endif

#ifdef WV_PLATFORM_WINDOWS
bool wv::Remote::isRunningRemoteTarget( int argc, char* argv[] )
{
	return false;
}
void wv::Remote::remoteMain() { }
void wv::Remote::remoteMainExit() { }
#endif

int main( int _argc, char* _argv[] )
{
	if ( !wv::Platform::initialize() )
		return 1;

	wv::Trace::Trace::printEnabled = false; // remove?
	wv::Error::init();

	if ( !wv::Console::isInitialized() )
		wv::Console::initialize();

	if ( wv::Remote::isRunningRemoteTarget( _argc, _argv ) )
	{
		wv::Debug::Print( "Launching Remote Client" );
		wv::Thread::sleepForSeconds( 3 );
		wv::Remote::remoteMain();
	}

	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Initializing Application Configuration\n" );

	wv::Application app;
	
#if defined( WV_PACKAGE ) and defined( __cpp_exceptions )
	try 
	{
#endif
		if ( app.initialize( WV_NEW( wv::World ), 900, 600 ) )
		{
			wv::Debug::Print( "Starting Run Loop\n" );

			bool running = true;

			while ( running )
				running = app.tick();

			wv::Debug::Print( "Ending Run Loop\n" );
			app.shutdown();
		}
#if defined( WV_PACKAGE ) and defined( __cpp_exceptions )
	}
	catch ( const std::runtime_error& re )
	{
		printf( "\n[========= Runtime Error Occured ==========]\n"
				"\n%s\n"
				"\n[==========================================]\n", re.what() );
	}
	catch ( const std::exception& ex )
	{
		printf( "\n[=========== Exception Occured ============]\n"
				"\n%s\n"
				"\n[==========================================]\n", ex.what() );
	}
	catch ( ... )
	{
		printf( "\n[============ Unknown Failure =============]\n"
				"\nPossible memory corruption\n\n"
				"\n[==========================================]\n" );
	}
#endif

	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Program Exit\n" );

	wv::Platform::cleanup();

	wv::MemoryTracker::dump();

#ifdef WV_PLATFORM_3DS
	wv::Thread::sleepForSeconds( 1 );
#endif

	if ( wv::Remote::isRunningRemoteTarget( _argc, _argv ) )
		wv::Remote::remoteMainExit();

	if ( wv::Console::isInitialized() )
		wv::Console::deinitialize();

	return 0;
}
