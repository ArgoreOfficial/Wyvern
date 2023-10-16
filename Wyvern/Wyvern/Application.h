#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/Window/Window.h>
#include <vendor/imgui_impl_bgfx.h>

#include <imgui_impl_glfw.h>
#include <imgui.h>



namespace WV
{
	class Game;

	class Application : ISingleton<Application>
	{
	public:
		static void run( Game* _game ) { getInstance().internalRun(_game); }
		static double getTime() { return getInstance().m_time; }

	private:
		static void init( Game* _game );
		static void deinit();

		void internalRun( Game* _game );
		
		
		void update();
		void draw();

		WV::Game* m_game = nullptr;
		WV::Window* m_window = nullptr;
		
		double m_lastTime = 0.0;
		double m_time = 0.0;
		float m_deltaTime = 0.0f;
	};
}
