#include <stdio.h>

#include "Window/cWindow.h"
#include "Renderer/cRenderer.h"

///////////////////////////////////////////////////////////////////////////////////////

int main()
{
	cm::cWindow window;
	window.create();

	cm::cRenderer renderer;
	renderer.init( &window );

	/* main loop */
	while ( !window.shouldClose() )
	{
		window.update();

		renderer.draw();
	}

	renderer.destroy();
	window.destroy();
}