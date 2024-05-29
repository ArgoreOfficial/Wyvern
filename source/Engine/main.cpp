
#include <wv/Application/Application.h>

int main()
{
	wv::ApplicationDesc appDesc;
	appDesc.title = "Wyvern";
	appDesc.vsync = true;
	appDesc.fullscreen = false;
	appDesc.windowWidth = 900;
	appDesc.windowHeight = 600;
	appDesc.showDebugConsole = true;

	wv::Application* app = new wv::Application( &appDesc );
	app->run();
	app->terminate();
}