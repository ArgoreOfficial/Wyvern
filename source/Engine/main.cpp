
#include <wv/Application/Application.h>
#include <App/StateGame.h>

int main()
{
	wv::ApplicationDesc appDesc;
	appDesc.title = "Wyvern";
	appDesc.vsync = true;
	appDesc.fullscreen = false;
	appDesc.windowWidth = 640 * 2;
	appDesc.windowHeight = 480 * 2;
	appDesc.showDebugConsole = true;
	appDesc.applicationState = new StateGame();

	wv::Application* app = new wv::Application( &appDesc );
	app->run();
	app->terminate();
}