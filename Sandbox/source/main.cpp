#include <Wyvern/Application.h>
#include <Wyvern/Game.h>
#include "DefaultGame.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
	SetConsoleTitleA( "Wyvern Log" );
#endif

	DefaultGame* game = new DefaultGame();
	WV::Application::run( game );
	delete game;

	return 0;
}