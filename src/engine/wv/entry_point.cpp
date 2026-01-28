#include "entry_point.h"

#include "wv/application.h"

#include <wv/console/console.h>
#include <wv/debug/error.h>
#include <wv/debug/trace.h>
#include <wv/memory/memory.h>
#include <wv/target/iremote_target.h>
#include <wv/entity/world.h>

#include <wv/platform/platform.h>
#include <wv/platform/thread.h>

#ifdef WV_PLATFORM_WINDOWS
bool wv::Remote::isRunningRemoteTarget( int argc, char* argv[] )
{
	return false;
}
void wv::Remote::remoteMain() { }
void wv::Remote::remoteMainExit() { }
#endif

int wv::entryPoint( World* _world, int _argc, char* _argv[] )
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

	if ( app.initialize( _world, 900, 600 ) )
	{
		wv::Debug::Print( "Starting Run Loop\n" );

		bool running = true;

		while ( running )
			running = app.tick();

		wv::Debug::Print( "Ending Run Loop\n" );
		app.shutdown();
	}

	wv::Debug::Print( wv::Debug::WV_PRINT_INFO, "Program Exit\n" );

	wv::Platform::cleanup();

	wv::MemoryTracker::dump();

	if ( wv::Remote::isRunningRemoteTarget( _argc, _argv ) )
		wv::Remote::remoteMainExit();

	if ( wv::Console::isInitialized() )
		wv::Console::deinitialize();

	return 0;
}
