#pragma once

#include <wv/Events/Events.h>

#include <wv/Math/Vector2.h>

#include <wv/Types.h>
#include <wv/Graphics/Types.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iLowLevelGraphics;
	class AudioDevice;

	class iIntermediateRenderTargetHandler;

	class iCamera;
	class cFileSystem;
	class cSceneRoot;
	class cModelObject;
	class iMaterial;
	
	class cApplicationState;
	class cShaderResource;

	class cResourceRegistry;
	class cJoltPhysicsEngine;
	class JobSystem;

	class UpdateManager;

///////////////////////////////////////////////////////////////////////////////////////

	struct EngineDesc
	{
		const char* title;
		
		int windowWidth  = 800;
		int windowHeight = 600;

		bool showDebugConsole = true;

		struct
		{
			iDeviceContext*    pContext;
			iLowLevelGraphics* pGraphics;
			AudioDevice*       pAudio;
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

	class cEngine
	{

	public:
	#ifdef WV_SUPPORT_JOLT_PHYSICS
		friend class cJoltPhysicsEngine;
	#endif

		cEngine( EngineDesc* _desc );
		static cEngine* get();
		
		static wv::UUID   getUniqueUUID();
		static wv::Handle getUniqueHandle();

		void onResize( int _width, int _height );
		void handleInput();

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
		cShaderResource* m_pDeferredShader = nullptr;
		RenderTargetID m_gbuffer{};
		
		// engine
		iDeviceContext*    context  = nullptr;
		iLowLevelGraphics* graphics = nullptr;
		AudioDevice*       audio    = nullptr;

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
	#ifdef WV_SUPPORT_JOLT_PHYSICS
		cJoltPhysicsEngine* m_pPhysicsEngine    = nullptr;
	#endif
		JobSystem*          m_pJobSystem        = nullptr;
		
		struct sFogParams
		{
			wv::Vector4f colorDensity{};
			int isEnabled = 0;
		} m_fogParams;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		void initImgui();
		void shutdownImgui();

		void createScreenQuad();
		void createGBuffer();

		void recreateScreenRenderTarget( int _width, int _height );

		void _physicsUpdate( double _deltaTime );

///////////////////////////////////////////////////////////////////////////////////////

		unsigned int m_fpsCounter = 0;
		double m_fpsAccumulator     = 0.0;
		double m_fpsUpdateInterval  = 0.5;
		double m_timeSinceFPSUpdate = 0.0;

		double m_averageFps = 0.0;
		double m_maxFps = 0.0;

		bool m_drawWireframe = false;

		wv::Vector2i m_mousePosition;
		cMouseEventListener m_mouseListener;
		cInputEventListener m_inputListener;

		static cEngine* s_pInstance; 
	};

}