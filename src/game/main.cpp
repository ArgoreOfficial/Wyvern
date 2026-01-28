#include <wv/entry_point.h>
#include <wv/memory/memory.h>

#include "game_world.h"

int main( int _argc, char* _argv[] )
{
	GameWorld* world = WV_NEW( GameWorld );
	wv::entryPoint( world, _argc, _argv );
	return 0;
}
