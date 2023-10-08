#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/Game.h>
#include <Wyvern/Window/Window.h>

namespace WV
{
	class Application : ISingleton<Application>
	{
	public:
		static void init( Game* _game );
		static void deinit();
	private:
		static void run();

		WV::Game* m_game = nullptr;
		WV::Window* m_window = nullptr;
	};
}
