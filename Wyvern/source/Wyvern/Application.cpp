#include "Application.h"
#include <Wyvern/Logging/Logging.h>

using namespace WV;

Application::Application()
{

}

Application::~Application()
{

}

void Application::init( Game* _game )
{
	int major = 0;
	int minor = 1;

	WVINFO( "Loading Wyvern runtime version %i.%i", major, minor );

	if ( _game == nullptr )
	{
		WVFATAL( "Game instance failed to create!" );
		return;
	}

	getInstance().m_game = _game;
	WVDEBUG( "Game instance created" );

	getInstance().m_window = new WV::Window();
	if ( !getInstance().m_window->createWindow() )
	{
		delete getInstance().m_window;
		return;
	}

	run();
}

Application& Application::getInstance()
{
	static Application instance;
	return instance;
}

void Application::deinit()
{

}

void drawtest()
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

		drawtest();
		game->update( 0.0f );
		game->draw();

		glfwSwapBuffers( window->getWindow() );
		run = window->pollEvents();
	}
}
