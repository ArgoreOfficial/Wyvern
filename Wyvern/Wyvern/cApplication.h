#pragma once
#include <Wyvern/Core/cLayerStack.h>
#include <Wyvern/Core/cSceneGraph.h>
#include <Wyvern/Core/iSingleton.h>
#include <Wyvern/Renderer/cViewport.h>
#include <Wyvern/Window/cWindow.h>
#include <vector>
#include "imgui.h"
#include "imgui_impl_opengl3.h"


namespace wv
{
	class iLayer;
	
	class cApplication : iSingleton<cApplication>
	{
	public:

		static void run();
		static void pushLayer( iLayer* _layer ) { getInstance().m_layerStack.push_back( _layer ); }
		
		static inline double       getTime()      { return getInstance().m_time; }
		static inline double       getDeltaTime() { return getInstance().m_deltaTime; }
		static inline cSceneGraph& getScene()     { return getInstance().m_sceneGraph; }
		static inline cViewport&   getViewport()  { return getInstance().m_viewport; }

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

		cViewport m_viewport;

		double m_lastTime = 0.0;
		double m_time = 0.0;
		double m_deltaTime = 0.0f;
	};
}
