#include "Sandbox.h"

#include <wv/Engine/ApplicationState.h>
#include <wv/Engine/Engine.h>

#include <wv/Device/AudioDevice.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Memory/FileSystem.h>

#include <wv/Scene/SceneRoot.h>

#include "SceneObjects/DemoWindow.h"

#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

REFLECT_CLASS( cDemoWindow );

///////////////////////////////////////////////////////////////////////////////////////


bool cSandbox::create( void )
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
	wv::ContextDesc ctxDesc;

#ifdef EMSCRIPTEN /// WebGL only supports OpenGL ES 2.0/3.0
	ctxDesc.deviceApi = wv::WV_DEVICE_CONTEXT_API_SDL;
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL_ES2;
	ctxDesc.graphicsApiVersion.major = 3;
	ctxDesc.graphicsApiVersion.minor = 0;
#elif defined( WV_PLATFORM_WINDOWS )
	ctxDesc.deviceApi   = wv::WV_DEVICE_CONTEXT_API_GLFW;
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL;
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

	wv::iDeviceContext* deviceContext = wv::iDeviceContext::getDeviceContext( &ctxDesc );
	if ( !deviceContext )
		return false;

	deviceContext->setSwapInterval( 1 ); // vsync on(1) off(0)

	// create graphics device
	wv::GraphicsDeviceDesc deviceDesc;
	deviceDesc.loadProc = deviceContext->getLoadProc();
	deviceDesc.pContext = deviceContext;
	
	wv::iGraphicsDevice* graphicsDevice = wv::iGraphicsDevice::createGraphicsDevice( &deviceDesc );
	if ( !graphicsDevice )
	{
		wv::Debug::Print( "Graphics Device was nullptr\n" );
		return false;
	}

	engineDesc.device.pContext = deviceContext;
	engineDesc.device.pGraphics = graphicsDevice;
	
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Initializing Audio Device\n" );
	engineDesc.device.pAudio = new wv::AudioDevice( nullptr );

	// create modules
	wv::cFileSystem* fileSystem = new wv::cFileSystem();

	// set up load directories
	fileSystem->addDirectory( "res/" );
	fileSystem->addDirectory( "res/materials/" );
	fileSystem->addDirectory( "res/meshes/" );
	fileSystem->addDirectory( "res/shaders/" );
	fileSystem->addDirectory( "res/textures/" );

	engineDesc.systems.pFileSystem = fileSystem;
	
	// setup application state
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Creating Application State\n" );
	wv::cApplicationState* appState = new wv::cApplicationState();
	engineDesc.pApplicationState = appState;

	// load scenes
	wv::cSceneRoot* scene = appState->loadScene( fileSystem, "res/scenes/playground.json" );
	appState->addScene( scene ); // the engine will load into scene 0 by default

	// create engine
	m_pEngine = new wv::cEngine( &engineDesc );

	if( !wv::cEngine::get() )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_FATAL, "Couldn't create Engine\n" );
		return false;
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void cSandbox::run( void )
{
	m_pEngine->run();
}

///////////////////////////////////////////////////////////////////////////////////////

void cSandbox::destroy( void )
{
	m_pEngine->terminate();
}

///////////////////////////////////////////////////////////////////////////////////////
