
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

struct IRuntimeCallableBase
{
	void operator()( wv::IRuntimeObjectBase* _obj, const std::vector<std::string>& _args ) {
		call( _obj, _args ); 
	}
	virtual void call( wv::IRuntimeObjectBase* _obj, const std::vector<std::string>& _args ) = 0;
	virtual int getArgCount() = 0;
};

template<typename... _Args>
struct IRuntimeMemberCallable : IRuntimeCallableBase
{
private:

	template<std::size_t... _S>
	void _handleCall( wv::IRuntimeObjectBase* _obj, std::index_sequence<_S...>, const std::vector<std::string>& _args );

public:
	typedef void( wv::IRuntimeObjectBase::* fptr_t )( _Args... );
	IRuntimeMemberCallable( fptr_t _fptr ) : 
		fptr{ _fptr } 
	{}

	template<typename _Ty>
	IRuntimeMemberCallable( void( _Ty::* _fptr )( _Args... ) ) : 
		fptr{ (fptr_t)_fptr } 
	{}

	virtual int getArgCount() override { return (int)sizeof...( _Args ); }

	virtual void call( wv::IRuntimeObjectBase* _obj, const std::vector<std::string>& _args ) override {
		_handleCall( _obj, std::index_sequence_for<_Args...>{}, _args );
	}

	fptr_t fptr;
};

template <typename T>
static T convertArg( const std::string& _str );

template<> int         convertArg<int>( const std::string& _str ) { return std::stoi( _str ); }
template<> std::string convertArg<std::string>( const std::string& _str ) { return _str; }

template<typename ..._Args>
template<std::size_t ..._S>
void IRuntimeMemberCallable<_Args...>::_handleCall( wv::IRuntimeObjectBase* _obj, std::index_sequence<_S...>, const std::vector<std::string>& _args )
{
	if( sizeof...( _Args ) != (int)_args.size() )
	{
		printf( "Error: Expected %i arguments, got %i", (int)getArgCount(), (int)_args.size() );
		return;
	}

	( _obj->*fptr )( convertArg<_Args>( _args.data()[ _S ] )... );
}

WV_RUNTIME_OBJECT( Hotel, IRuntimeObject )
class Hotel : public wv::IRuntimeObject<Hotel>
{
public:
	void occupyRoom( int _room ) {
		printf( "Occupied room nr.%i\n", _room );
	}
	
	int availableRooms = 6;

	static wv::RuntimeMethods queryMethods() {
		wv::RuntimeMethods m;
		m.methods = {
			"occupyRoom"
		};
		return m;
	}

	static void queryProperties( wv::RuntimeProperties* _pOutProps ) {
		_pOutProps->add( "availableRooms", &Hotel::availableRooms );
	}
};

int main( int argc, char* argv[] )
{
	wv::RuntimeDataBase* database = wv::RuntimeDataBase::get();
	database->dump();
	
	Hotel test;
	int availableRooms = test.getProperty<int>( "availableRooms" ); // 6, default
	test.setProperty<int>( "availableRooms", 23 );
	availableRooms = test.getProperty<int>( "availableRooms" ); // 23
	
	IRuntimeMemberCallable asd{ &Hotel::occupyRoom };
	asd.call( &test, { "5" } );
	
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
