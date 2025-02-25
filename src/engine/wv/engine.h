#pragma once

#include <wv/event/events.h>

#include <wv/math/vector2.h>

#include <wv/types.h>
#include <wv/graphics/types.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IDeviceContext;
	class IGraphicsDevice;
	class AudioDevice;

	class ICamera;
	class IFileSystem;
	class SceneRoot;
	class ModelObject;
	class IMaterial;
	
	class ApplicationState;
	class ShaderResource;

	class ResourceRegistry;
	class JoltPhysicsEngine;
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
			IDeviceContext*    pContext;
			IGraphicsDevice* pGraphics;
			AudioDevice*       pAudio;
		} device;

		struct
		{
			IFileSystem* pFileSystem;
		} systems;

		ApplicationState* pApplicationState = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Engine
	{

	public:
	#ifdef WV_SUPPORT_JOLT_PHYSICS
		friend class JoltPhysicsEngine;
	#endif

		Engine( EngineDesc* _desc );
		static Engine* get();
		
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
		ShaderResource* m_pDeferredShader = nullptr;
		RenderTargetID m_gbuffer{};
		
		// engine
		IDeviceContext*    context  = nullptr;
		IGraphicsDevice* graphics = nullptr;
		AudioDevice*       audio    = nullptr;

		// camera 
		/// TODO: move to applicationstate
		ICamera* currentCamera    = nullptr;
		ICamera* orbitCamera      = nullptr;
		ICamera* freeflightCamera = nullptr;

		RenderTargetID m_screenRenderTarget{};

		ApplicationState* m_pApplicationState = nullptr;

		// modules
		IFileSystem*        m_pFileSystem       = nullptr;
		ResourceRegistry*  m_pResourceRegistry = nullptr;
	
		JoltPhysicsEngine* m_pPhysicsEngine    = nullptr;
		
		JobSystem*          m_pJobSystem        = nullptr;
		
		struct FogParams
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
		MouseEventListener m_mouseListener;
		InputEventListener m_inputListener;

		static Engine* s_pInstance; 
	};

}