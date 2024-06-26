
#include <wv/Engine/Engine.h>
#include <StateGame.h>

///////////////////////////////////////////////////////////////////////////////////////

int main()
{
	wv::EngineDesc appDesc;
	appDesc.title = "Wyvern";
	appDesc.vsync = true;
	appDesc.fullscreen = false;
	appDesc.windowWidth  = 640;
	appDesc.windowHeight = 480;
	appDesc.showDebugConsole = true;
	appDesc.applicationState = new StateGame();
	appDesc.allowResize = false;

	wv::cEngine* app = new wv::cEngine( &appDesc );
	app->run();
	app->terminate();
}