#pragma once

#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/Window/Window.h>
#include <Wyvern/Camera.h>

#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <vendor/imgui_impl_bgfx.h>

namespace WV
{
	class ILayer;
	class LayerStack;

	class Application : ISingleton<Application>
	{
	public:
		static void run( ILayer* _gameLayer );
		
		static double getTime() { return getInstance().m_time; }
		static double getDeltaTime() { return getInstance().m_deltaTime; }
		
		static void windowResizeCallback( GLFWwindow* _window, int _width, int _height );
		static WV::Window* getWindow() { return getInstance().m_window; }
		
		// temp?
		static void setActiveCamera( Camera* _camera ) { getInstance().m_activeCamera = _camera; }

		void pushLayer( ILayer* _layer ) { m_layerStack->push_back( _layer ); }
		void startLoadThread();
	private:
		
		int create( );
		void destroy();
		void internalRun();
		void update();
		void draw();
		void shutdown();

		void initImgui();

		LayerStack* m_layerStack;
		Window* m_window = nullptr;
		ImGuiIO* m_io;
		Camera* m_activeCamera;

		double m_lastTime = 0.0;
		double m_time = 0.0;
		double m_deltaTime = 0.0f;
	};
}
