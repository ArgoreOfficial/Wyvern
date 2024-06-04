#pragma once

#include <wv/Events/IMouseListener.h>
#include <wv/Events/IInputListener.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Context;
	class GraphicsDevice;
	class Mesh;
	class Pipeline;
	class ICamera;
	class RenderTarget;
	class MemoryDevice;
	class RootNode;
	class Model;

///////////////////////////////////////////////////////////////////////////////////////

	struct ApplicationDesc
	{
		const char* title;
		
		bool vsync = true;
		bool fullscreen = false;
		int windowWidth = 800;
		int windowHeight = 600;

		bool showDebugConsole = true;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Application : IMouseListener, IInputListener
	{

	public:

		Application( ApplicationDesc* _desc );
		static Application* get();

		void run();
		void terminate();
		void tick();

		void onResize( int _width, int _height );

		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( InputEvent _event ) override;

///////////////////////////////////////////////////////////////////////////////////////

		/// TEMPORARY---
		Model* m_mesh;
		Model* m_skyBox;
		/// ---TEMPORARY

		// deferred rendering
		Mesh*         m_screenQuad       = nullptr;
		Pipeline*     m_deferredPipeline = nullptr;
		RenderTarget* m_gbuffer          = nullptr;

		// engine
		Context*        context = nullptr;
		GraphicsDevice* device  = nullptr;
		MemoryDevice*   memoryDevice = nullptr;

		// camera 
		/// TODOM: move?
		ICamera* currentCamera    = nullptr;
		ICamera* orbitCamera      = nullptr;
		ICamera* freeflightCamera = nullptr;

		/*
		 * Special render target with handle 0
		 * targetting the default context
		 * backbuffer.
		 */
		RenderTarget* m_defaultRenderTarget = nullptr;

	private:

		void createDeferredPipeline();
		void createScreeQuad();
		void createGBuffer();

		float m_maxFps = 0.0f;
		
	#define FPS_CACHE_NUM 4000
		float m_fpsCache[ FPS_CACHE_NUM ] = { 0.0f };
		int m_fpsCacheCounter = 0;
		float m_averageFps = 0.0f;

		RootNode* m_scene;

		/*
		 * technically not a singleton but getting a reference 
		 * to the application can sometimes be very useful.
		 * 
		 * this will have to change in case multiple applications 
		 * are to be supported
		 */
		static inline Application* s_instance = nullptr; 
	};
}