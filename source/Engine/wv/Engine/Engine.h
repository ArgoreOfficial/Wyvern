#pragma once

#include <wv/Events/MouseListener.h>
#include <wv/Events/InputListener.h>
#include <wv/Math/Vector2.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iGraphicsDevice;
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

	struct EngineDesc
	{
		const char* title;
		
		bool vsync = true;
		bool fullscreen = false;
		int windowWidth = 800;
		int windowHeight = 600;
		bool allowResize = false;

		bool showDebugConsole = true;

		State* applicationState = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cEngine : IMouseListener, IInputListener
	{

	public:

		cEngine( EngineDesc* _desc );
		static cEngine* get();

		void onResize( int _width, int _height );
		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( InputEvent _event ) override;

		void setSize( int _width, int _height, bool _notify = true );

		wv::Vector2i getMousePosition() { return m_mousePosition; }

		void run();
		void terminate();
		void tick();
		void quit();

///////////////////////////////////////////////////////////////////////////////////////

		// deferred rendering
		Mesh*         m_screenQuad       = nullptr;
		Pipeline*     m_deferredPipeline = nullptr;
		RenderTarget* m_gbuffer          = nullptr;

		// engine
		iDeviceContext*  context = nullptr;
		iGraphicsDevice* device  = nullptr;
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
		
	#define FPS_CACHE_NUM 200
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
		static inline cEngine* s_instance = nullptr; 

		wv::Vector2i m_mousePosition;

	};

}