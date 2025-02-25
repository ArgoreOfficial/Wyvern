#include "Sandbox.h"

#include <wv/app_state.h>
#include <wv/engine.h>

#include <wv/device/audio_device.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/memory/memory.h>
#include <wv/filesystem/file_system.h>
#include <noapi/noapi_file_system.h>

#include <wv/scene/scene.h>

#include "scene/demo_window.h"


#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

REFLECT_CLASS( DemoWindow );

///////////////////////////////////////////////////////////////////////////////////////


bool Sandbox::create( void )
{
	wv::EngineDesc engineDesc;

#ifdef WV_PLATFORM_PSVITA
	engineDesc.windowWidth  = wv::PSVita::DISPLAY_WIDTH;
	engineDesc.windowHeight = wv::PSVita::DISPLAY_HEIGHT;
#else
	engineDesc.windowWidth  = 1280;
	engineDesc.windowHeight = 960;
	engineDesc.showDebugConsole = true;
#endif

	// create device context
	wv::ContextDesc ctxDesc{};

#ifdef EMSCRIPTEN /// WebGL only supports OpenGL ES 2.0/3.0
	ctxDesc.deviceApi = wv::WV_DEVICE_CONTEXT_API_SDL;
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL_ES2;
	ctxDesc.graphicsApiVersion.major = 3;
	ctxDesc.graphicsApiVersion.minor = 0;
#elif defined( WV_PLATFORM_WINDOWS )
#ifdef WV_SUPPORT_SDL2
	ctxDesc.deviceApi   = wv::WV_DEVICE_CONTEXT_API_SDL;
#endif
#ifdef WV_SUPPORT_OPENGL
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL;
#endif
	ctxDesc.graphicsApiVersion.major = 4;
	ctxDesc.graphicsApiVersion.minor = 6;
#elif defined( WV_PLATFORM_PSVITA )
	ctxDesc.deviceApi   = wv::WV_DEVICE_CONTEXT_API_PSVITA;
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_PSVITA;
	ctxDesc.graphicsApiVersion.major = 3;
	ctxDesc.graphicsApiVersion.minor = 57;
#endif

	ctxDesc.name   = "Wyvern Sandbox";
	ctxDesc.width  = engineDesc.windowWidth;
	ctxDesc.height = engineDesc.windowHeight;
	ctxDesc.allowResize = false;

	wv::IDeviceContext* deviceContext = wv::IDeviceContext::getDeviceContext( &ctxDesc );
	if ( !deviceContext )
	{
		wv::Debug::Print( "Device Context was nullptr\n" );
		return false;
	}

	deviceContext->setSwapInterval( 0 ); // vsync on(1) off(0)

	// create graphics device
	wv::GraphicsDeviceDesc GraphicsDeviceDesc;
	GraphicsDeviceDesc.loadProc = deviceContext->getLoadProc();
	GraphicsDeviceDesc.pContext = deviceContext;
	
	wv::IGraphicsDevice* pGraphicsDevice = wv::IGraphicsDevice::createGraphics( &GraphicsDeviceDesc );
	if ( !pGraphicsDevice )
	{
		wv::Debug::Print( "Graphics Device was nullptr\n" );
		return false;
	}

	engineDesc.device.pContext = deviceContext;
	engineDesc.device.pGraphics = pGraphicsDevice;
	
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Initializing Audio Device\n" );
	engineDesc.device.pAudio = WV_NEW( wv::AudioDevice, nullptr );

	// create modules
	wv::IFileSystem* fileSystem = WV_NEW( wv::NoAPIFileSystem );
	// set up load directories
	fileSystem->addDirectory( "materials/" );
	fileSystem->addDirectory( "meshes/" );
	fileSystem->addDirectory( "shaders/user/" );
	fileSystem->addDirectory( "shaders/core/" );
	fileSystem->addDirectory( "textures/" );

	engineDesc.systems.pFileSystem = fileSystem;
	
	// setup application state
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Creating Application State\n" );
	wv::ApplicationState* appState = WV_NEW( wv::ApplicationState );
	engineDesc.pApplicationState = appState;

	// create engine
	m_pEngine = WV_NEW( wv::Engine, &engineDesc );

	// load scenes
	wv::Scene* scene = appState->loadScene( fileSystem, "scenes/playground.json" );
	appState->addScene( scene ); // the engine will load into scene 0 by default
	
	// set fog params 
	/// TODO: move to scene
	m_pEngine->m_fogParams.colorDensity = { 0.85f, 0.f, 0.f, 0.15f };
	m_pEngine->m_fogParams.isEnabled = 0;

	if( !wv::Engine::get() )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_FATAL, "Couldn't create Engine\n" );
		return false;
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void Sandbox::run( void )
{
	m_pEngine->run();
}

///////////////////////////////////////////////////////////////////////////////////////

void Sandbox::destroy( void )
{
	m_pEngine->terminate();
	WV_FREE( m_pEngine );
	m_pEngine = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////
