#pragma once
#include "Wyvern/Core/iSingleton.h"
#include "Wyvern/Core/cLayerStack.h"
#include "Wyvern/Window/cWindow.h"
#include "Wyvern/cCamera.h"
#include "Wyvern/Core/cSceneGraph.h"
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace WV
{
	class iLayer;
	
	class cApplication : iSingleton<cApplication>
	{
	public:
		static void run();
		static void windowResizeCallback( GLFWwindow* _window, int _width, int _height );
		static void pushLayer( iLayer* _layer ) { getInstance().m_layerStack.push_back( _layer ); }
		
		static double getTime() { return getInstance().m_time; }
		static double getDeltaTime() { return getInstance().m_deltaTime; }
		static cSceneGraph& getScene() { return getInstance().m_sceneGraph; }
		static WV::cWindow& getWindow() { return getInstance().m_window; }

		// temp, move to renderer
		static void setActiveCamera( cCamera* _camera ) { getInstance().m_activeCamera = _camera; }

		void startLoadThread();

	private:
		int create( );
		void destroy();
		void internalRun();
		void update();
		void draw();
		void shutdown();

		void initImgui();

		cLayerStack m_layerStack;
		cSceneGraph m_sceneGraph;

		cWindow m_window;
		// temp, move to renderer
		cCamera* m_activeCamera = nullptr;
		
		double m_lastTime = 0.0;
		double m_time = 0.0;
		double m_deltaTime = 0.0f;
	};
}
