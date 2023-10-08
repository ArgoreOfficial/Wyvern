#pragma once
#include <Wyvern/Game.h>
#include <Wyvern/Window/Window.h>



namespace WV
{
	class Application
	{
	public:
		Application();
		~Application();

		static void init( Game* _game );
		static Application& getInstance();
		static void deinit();
	private:
		static void run();

		WV::Game* m_game = nullptr;
		WV::Window* m_window = nullptr;
	};
}
