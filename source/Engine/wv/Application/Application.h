#pragma once

#include <wv/Events/IMouseListener.h>
#include <wv/Events/IInputListener.h>
#include <wv/Math/Vector2.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class DeviceContext;
	class GraphicsDevice;
	class AudioDevice;

	class Mesh;
	class Pipeline;
	class ICamera;
	class RenderTarget;
	class MemoryDevice;
	class RootNode;
	class Model;
	class Material;
	class State;

///////////////////////////////////////////////////////////////////////////////////////

	struct ApplicationDesc
	{
		const char* title;
		
		bool vsync = true;
		bool fullscreen = false;
		int windowWidth = 800;
		int windowHeight = 600;

		bool showDebugConsole = true;

		State* applicationState = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Application : IMouseListener, IInputListener
	{

	public:

		Application( ApplicationDesc* _desc );
		static Application* get();

		void onResize( int _width, int _height );
		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( InputEvent _event ) override;

		wv::Vector2i getMousePosition() { return m_mousePosition; }

		void run();
		void terminate();
		void tick();

///////////////////////////////////////////////////////////////////////////////////////

		// deferred rendering
		Mesh*         m_screenQuad       = nullptr;
		Pipeline*     m_deferredPipeline = nullptr;
		RenderTarget* m_gbuffer          = nullptr;

		// engine
		DeviceContext*  context = nullptr;
		GraphicsDevice* device  = nullptr;
		AudioDevice*    audio   = nullptr;

		// camera 
		ICamera* currentCamera    = nullptr;
		/// TODOM: move?
		ICamera* orbitCamera      = nullptr;
		ICamera* freeflightCamera = nullptr;

		/*
		 * Special render target with handle 0
		 * targetting the default context
		 * backbuffer.
		 */
		RenderTarget* m_defaultRenderTarget = nullptr;

		State* m_applicationState = nullptr;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		void createScreeQuad();
		void createGBuffer();

///////////////////////////////////////////////////////////////////////////////////////

		float m_maxFps = 0.0f;
		
	#define FPS_CACHE_NUM 1000
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

		wv::Vector2i m_mousePosition;
	};

}