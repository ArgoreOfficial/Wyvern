#pragma once

#include <wv/Events/IMouseListener.h>
#include <wv/Events/IInputListener.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Context;
	class GraphicsDevice;
	class Primitive;
	class Pipeline;
	class ICamera;
	class Texture;
	class RenderTarget;

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
		static Application* getApplication();

		void run();
		void terminate();
		void tick();

		void onResize( int _width, int _height );

		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( InputEvent _event ) override;

///////////////////////////////////////////////////////////////////////////////////////

		/// TEMPORARY---
		Primitive* m_primitive = nullptr; // cube
		Pipeline*  m_pipeline  = nullptr; // cube shader
		Texture*   m_texture   = nullptr; // cube texture
		/// ---TEMPORARY

		// deferred rendering
		Primitive* m_screenQuad = nullptr;
		Pipeline* m_deferredPipeline = nullptr;
		RenderTarget* m_gbuffer = nullptr;

		// engine
		Context*        context = nullptr;
		GraphicsDevice* device  = nullptr;

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