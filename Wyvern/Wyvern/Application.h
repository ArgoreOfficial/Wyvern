#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/Window/Window.h>
#include <vendor/imgui_impl_bgfx.h>
#include <Wyvern/Camera.h>
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
		static double getDeltaTime() { return getInstance().m_deltaTime; }

		static void windowResizeCallback( GLFWwindow* _window, int _width, int _height );

		static WV::Window* getWindow() { return getInstance().m_window; }
		static void setActiveCamera( Camera* _camera ) { getInstance().m_activeCamera = _camera; }
	private:

		void init( Game* _game );
		void deinit();
		void initImgui();
		void styleImgui();
		void internalRun( Game* _game );
		void update();
		void draw();
		
		WV::Game* m_game = nullptr;
		WV::Window* m_window = nullptr;
		ImGuiIO* m_io;
		Camera* m_activeCamera;

		double m_lastTime = 0.0;
		double m_time = 0.0;
		float m_deltaTime = 0.0f;
	};
}
