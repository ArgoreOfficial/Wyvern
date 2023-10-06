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

	WVINFO( "Loading wyvern runtime version %i.%i", major, minor );
	
	if ( _game == nullptr )
	{
		WVFATAL( "Game instance failed to create!" );
		return;
	}
	else
	{
		Wyvern::getInstance().m_game = _game;
		WVDEBUG( "Game instance created" );
	}

	Wyvern::getInstance().m_window = new WV::Window();
	if ( !Wyvern::getInstance().m_window->createWindow() )
	{
		delete Wyvern::getInstance().m_window;
		return;
	}

}

Wyvern& Wyvern::getInstance()
{
	static Wyvern instance;
	return instance;
}

void Wyvern::deinit()
{

}

void Wyvern::run()
{
	Game* game = Wyvern::getInstance().m_game;
	Window* window = Wyvern::getInstance().m_window;

	game->load();
	bool run = true;
	while ( run )
	{
		window->processInput();

		game->update( 0.0f );
		game->draw();

		glfwSwapBuffers( window->getWindow() );
		run = window->pollEvents();
	}
}
