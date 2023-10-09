#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/Game.h>
#include <Wyvern/Window/Window.h>

namespace WV
{
	class Application : ISingleton<Application>
	{
	public:
		static void run( Game* _game ) { getInstance().internalRun(_game); }
		static double getTime() { return getInstance().m_time; }

	private:
		static void init( Game* _game );
		static void deinit();

		void internalRun( Game* _game );
		
		WV::Game* m_game = nullptr;
		WV::Window* m_window = nullptr;
		double m_time = 0.0;
	};
}
