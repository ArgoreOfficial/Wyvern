#include <Wyvern/Application.h>
#include <Wyvern/Game.h>
#include <Wyvern/DefaultGame.h>


#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
	SetConsoleTitleA( "Wyvern Log" );
#endif

	WV::DefaultGame* game = new WV::DefaultGame();
	WV::Application::init( game );
	delete game;

	return 0;
}