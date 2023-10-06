#pragma once
#include <Wyvern/API/Core.h>
#include <Wyvern/Game.h>
#include <Wyvern/Window/Window.h>



namespace WV
{
	class WYVERN_API Wyvern
	{
	public:
		Wyvern();
		~Wyvern();

		static void init( Game* _game );
		static Wyvern& getInstance();
		static void deinit();
	private:
		static void run();

		WV::Game* m_game = nullptr;
		WV::Window* m_window = nullptr;
	};
}
