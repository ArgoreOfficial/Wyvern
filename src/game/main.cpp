#include "wv/application.h"

#include <wv/console/console.h>
#include <wv/debug/error.h>
#include <wv/memory/memory.h>
#include <wv/platform/platform.h>
#include <wv/target/iremote_target.h>
#include <wv/thread/thread.h>

#include "game_world.h"

#ifdef WV_PLATFORM_WINDOWS
bool wv::Remote::isRunningRemoteTarget( int argc, char* argv[] ) { return false; }
void wv::Remote::initializeRemoteTarget() { }
void wv::Remote::shutdownRemoteTarget() { }
#endif

int main( int _argc, char* _argv[] )
{
	if ( !wv::Platform::initialize() )
		return 1;

	wv::Error::init();

	if ( !wv::Console::isInitialized() )
		wv::Console::initialize();

	const bool isRemoteTarget = wv::Remote::isRunningRemoteTarget( _argc, _argv );
	if ( isRemoteTarget )
	{
		wv::Debug::Print( "Launching Remote Client" );
		wv::Thread::sleepForSeconds( 3 );
		wv::Remote::initializeRemoteTarget();
	}

	wv::Debug::Print( "Initializing Application Configuration\n" );

	wv::Application app{};
	GameWorld* world = WV_NEW( GameWorld );

	if ( app.initialize( world, 1280, 720 ) )
	{
		wv::Debug::Print( "Starting Run Loop\n" );

		bool running = true;

		while ( running )
			running = app.tick();

		wv::Debug::Print( "Ending Run Loop\n" );
		app.shutdown();
	}

	wv::Debug::Print( "Program Exit\n" );

	wv::Platform::cleanup();

	wv::MemoryTracker::dump();

	if ( isRemoteTarget )
		wv::Remote::shutdownRemoteTarget();

	if ( wv::Console::isInitialized() )
		wv::Console::deinitialize();

	return 0;
}
