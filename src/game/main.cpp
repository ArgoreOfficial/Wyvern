#include "wv/application.h"

int main( int _argc, char* _argv[] )
{
	wv::Application app;
	app.initialize();

	bool running = true;
	while ( running )
		running = app.tick();
	
	app.shutdown();

	return 0;
}
