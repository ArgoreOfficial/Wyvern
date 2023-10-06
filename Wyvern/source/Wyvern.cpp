#include "Wyvern.h"
#include <Wyvern/Log/Log.h>

using namespace WV;

Wyvern::Wyvern()
{

}

Wyvern::~Wyvern()
{

}

void Wyvern::init( Game* _game )
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
	if ( !Wyvern::getInstance().m_window->createWindow() )
	{
		delete getInstance().m_window;
		return;
	}

	run();
}

Wyvern& Wyvern::getInstance()
{
	static Wyvern instance;
	return instance;
}

void Wyvern::deinit()
{

}

void drawtest()
{
	
}

void Wyvern::run()
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
