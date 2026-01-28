#include <wv/entry_point.h>
#include <wv/entity/world.h>

#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
WV_PSVITA_THREAD( "Sandbox" );
WV_PSVITA_HEAPSIZE( 1 * 1024 * 1024 );
#endif

int main( int _argc, char* _argv[] )
{
	wv::World* world = new wv::World();
	wv::entryPoint( world, _argc, _argv );
	return 0;
}
