
#include "Sandbox.h"

#include <wv/debug/trace.h>
#include <wv/memory/memory.h>
#include <wv/console/console.h>

#include <wv/target/iremote_target.h>

#include <wv/runtime.h>

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

#ifdef WV_PLATFORM_WINDOWS
bool wv::Remote::isRunningRemoteTarget( int argc, char* argv[] )
{
	return false;
}
void wv::Remote::remoteMain() { }
void wv::Remote::remoteMainExit() { }
#endif

WV_RUNTIME_OBJECT( Hotel, RuntimeObject )
class Hotel : public wv::RuntimeObject<Hotel>
{
public:
	void occupyRoom( int _room ) {
		printf( "Occupied room nr.%i\n", _room );
	}
	void addAndPrintTwoNumbers( int _a, int _b ) {
		printf( "%i + %i = %i\n", _a, _b, _a + _b );
	}
	
	int availableRooms = 6;

	static void queryProperties( wv::RuntimeProperties* _pOutProps ) {
		_pOutProps->add( "availableRooms", &Hotel::availableRooms );
	}

	static void queryFunctions( wv::RuntimeFunctions* _pOutFuncs ) {
		_pOutFuncs->add( "occupyRoom",            &Hotel::occupyRoom );
		_pOutFuncs->add( "addAndPrintTwoNumbers", &Hotel::addAndPrintTwoNumbers );
	}
};

int main( int argc, char* argv[] )
{
	wv::IRuntimeObject* test = wv::RuntimeRegistry::get()->instantiate( "Hotel" );
	if( test )
	{
		int availableRooms = test->getProperty<int>( "availableRooms" ); // 6, default
		test->setProperty<int>( "availableRooms", 23 );
		availableRooms = test->getProperty<int>( "availableRooms" ); // 23
	
		test->callFunction( "occupyRoom", { "4" } );
		test->callFunction( "addAndPrintTwoNumbers", { "4", "17" } );
		printf( "y\n" );
	}

	wv::Trace::Trace::printEnabled = false;

	if( !wv::Console::isInitialized() )
		wv::Console::initialize();

	if( wv::Remote::isRunningRemoteTarget( argc, argv ) )
	{
		wv::Debug::Print( "Launching Remote Client" );
		std::this_thread::sleep_for( std::chrono::seconds( 3 ) );

		wv::Remote::remoteMain();
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

	if( wv::Remote::isRunningRemoteTarget( argc, argv ) )
		wv::Remote::remoteMainExit();
	
	if ( wv::Console::isInitialized() )
		wv::Console::deinitialize();

	return 0;
}
