#pragma once

#include <Wyvern/Core/cLayerStack.h>
#include <Wyvern/Core/cSceneGraph.h>
#include <Wyvern/Core/iSingleton.h>
#include <Wyvern/Renderer/cViewport.h>

#include <vector>

#include "imgui.h"
#include "imgui_impl_opengl3.h"

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iLayer;
	
///////////////////////////////////////////////////////////////////////////////////////

	class cApplication : iSingleton<cApplication>
	{

	public:

		static void run( void );
		static void pushLayer( iLayer* _layer ) { getInstance().m_layerStack.push_back( _layer ); }
		
///////////////////////////////////////////////////////////////////////////////////////

		static double       getTime     ( void ) { return getInstance().m_time; }
		static double       getDeltaTime( void ) { return getInstance().m_deltaTime; }
		static cSceneGraph& getScene    ( void ) { return getInstance().m_sceneGraph; }
		static cViewport&   getViewport ( void ) { return getInstance().m_viewport; }

///////////////////////////////////////////////////////////////////////////////////////

		void startLoadThread();

///////////////////////////////////////////////////////////////////////////////////////

	private:

		int  create     ( void );
		void destroy    ( void );
		void initImgui  ( void ); // move?
		void internalRun( void );
		void update     ( void );
		void draw       ( void );

///////////////////////////////////////////////////////////////////////////////////////

		cLayerStack m_layerStack;
		cSceneGraph m_sceneGraph;
		cViewport   m_viewport;

		double      m_lastTime  = 0.0;
		double      m_time      = 0.0;
		double      m_deltaTime = 0.0f;

	};

///////////////////////////////////////////////////////////////////////////////////////

}
