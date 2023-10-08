#include "Application.h"
#include <Wyvern/Logging/Logging.h>

using namespace WV;


void Application::init( Game* _game )
{
	Application& instance = getInstance();
	int major = 0;
	int minor = 1;

	WVINFO( "Loading Wyvern runtime version %i.%i", major, minor );

	if ( _game == nullptr )
	{
		WVFATAL( "Game instance failed to create!" );
		return;
	}

	instance.m_game = _game;
	WVDEBUG( "Game instance created" );

	instance.m_window = new WV::Window();
	if ( !instance.m_window->createWindow() )
	{
		delete instance.m_window;
		return;
	}

	run();
}

void Application::deinit()
{

}

void Application::run()
{
	Game* game = getInstance().m_game;
	Window* window = getInstance().m_window;

	game->load();
	bool run = true;
	while ( run )
	{
		window->processInput();
		glClearColor( 0.4f, 0.4f, 0.6f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		game->update( 0.0f );
		game->draw();

		glfwSwapBuffers( window->getWindow() );
		run = window->pollEvents();
	}
}
