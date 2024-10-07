#pragma once

#include <wv/Events/MouseListener.h>
#include <wv/Events/InputListener.h>

#include <wv/Math/Vector2.h>

#include <wv/Types.h>
#include <wv/Graphics/Types.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iGraphicsDevice;
	class AudioDevice;

	class iIntermediateRenderTargetHandler;

	class iCamera;
	struct sRenderTarget;
	class cFileSystem;
	class cSceneRoot;
	class cModelObject;
	class iMaterial;
	class sMesh;

	class cApplicationState;

	class cPipelineResource;

	class cResourceRegistry;
	class cJoltPhysicsEngine;

///////////////////////////////////////////////////////////////////////////////////////

	struct EngineDesc
	{
		const char* title;
		
		int windowWidth  = 800;
		int windowHeight = 600;

		bool showDebugConsole = true;

		struct
		{
			iDeviceContext*  pContext;
			iGraphicsDevice* pGraphics;
			AudioDevice*     pAudio;
		} device;

		struct
		{
			cFileSystem* pFileSystem;
		} systems;

		/// <summary>
		/// Optional intermediate render target. Can be used to render to viewport
		/// </summary>
		iIntermediateRenderTargetHandler* pIRTHandler = nullptr;

		cApplicationState* pApplicationState = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cEngine : iMouseListener, iInputListener
	{

	public:

		cEngine( EngineDesc* _desc );
		static cEngine* get();
		
		static wv::UUID   getUniqueUUID();
		static wv::Handle getUniqueHandle();

		void onResize( int _width, int _height );
		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( sInputEvent _event ) override;

		void setSize( int _width, int _height, bool _notify = true );
		void setDrawWireframe( bool _enabled ) { m_drawWireframe = _enabled; }

		wv::Vector2i getMousePosition() { return m_mousePosition; }

		wv::Vector2i getViewportSize();
		float getViewportAspect()
		{
			wv::Vector2i size = getViewportSize();
			return ( float )size.x / ( float )size.y;
		}

		void run();
		void terminate();
		void tick();
		void quit();

///////////////////////////////////////////////////////////////////////////////////////

		// deferred rendering
		MeshID m_screenQuad{};
		cPipelineResource* m_deferredPipeline = nullptr;
		RenderTargetID m_gbuffer{};
		
		// engine
		iDeviceContext*  context  = nullptr;
		iGraphicsDevice* graphics = nullptr;
		AudioDevice*     audio    = nullptr;

		// camera 
		/// TODO: move to applicationstate
		iCamera* currentCamera    = nullptr;
		iCamera* orbitCamera      = nullptr;
		iCamera* freeflightCamera = nullptr;

		iIntermediateRenderTargetHandler* m_pIRTHandler = nullptr;
		RenderTargetID m_screenRenderTarget{};

		cApplicationState* m_pApplicationState = nullptr;

		// modules
		cFileSystem*        m_pFileSystem       = nullptr;
		cResourceRegistry*  m_pResourceRegistry = nullptr;
		cJoltPhysicsEngine* m_pPhysicsEngine    = nullptr;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		void initImgui();
		void shutdownImgui();

		void createScreenQuad();
		void createGBuffer();

		void recreateScreenRenderTarget( int _width, int _height );

///////////////////////////////////////////////////////////////////////////////////////

		unsigned int m_fpsCounter = 0;
		double m_fpsAccumulator     = 0.0;
		double m_fpsUpdateInterval  = 0.5;
		double m_timeSinceFPSUpdate = 0.0;

		double m_averageFps = 0.0;
		double m_maxFps = 0.0;

		bool m_drawWireframe = false;

		wv::Vector2i m_mousePosition;

		static cEngine* s_pInstance; 
	};

}