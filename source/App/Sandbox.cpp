#include "Sandbox.h"

#include <wv/Engine/ApplicationState.h>
#include <wv/Engine/Engine.h>

#include <wv/Device/AudioDevice.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Memory/MemoryDevice.h>
#include <wv/Shader/ShaderRegistry.h>

#include <wv/Scene/SceneRoot.h>
#include <wv/Scene/Skybox.h>
#include <wv/Scene/Rigidbody.h>

#include <wv/Physics/PhysicsBodyDescriptor.h>

#include "SceneObjects/TentacleSection.h"
#include "SceneObjects/TentacleSettingWindow.h"

///////////////////////////////////////////////////////////////////////////////////////

bool cSandbox::create( void )
{
	wv::EngineDesc engineDesc;
	engineDesc.windowWidth = 640 * 2;
	engineDesc.windowHeight = 480 * 2;
	engineDesc.showDebugConsole = true;

	// create device context
	wv::ContextDesc ctxDesc;

#ifdef EMSCRIPTEN /// WebGL only supports OpenGL ES 2.0/3.0
	ctxDesc.deviceApi = wv::WV_DEVICE_CONTEXT_API_SDL;
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL_ES2;
	ctxDesc.graphicsApiVersion.major = 3;
	ctxDesc.graphicsApiVersion.minor = 0;
#else
	ctxDesc.deviceApi = wv::WV_DEVICE_CONTEXT_API_GLFW;
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL;
	ctxDesc.graphicsApiVersion.major = 3;
	ctxDesc.graphicsApiVersion.minor = 1;
#endif

	ctxDesc.name   = "Wyvern Sandbox";
	ctxDesc.width  = engineDesc.windowWidth;
	ctxDesc.height = engineDesc.windowHeight;
	ctxDesc.allowResize = false;

	wv::iDeviceContext* deviceContext = wv::iDeviceContext::getDeviceContext( &ctxDesc );
	if ( !deviceContext )
		return false;

	deviceContext->setSwapInterval( 0 ); // disable vsync

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
	wv::Debug::Print( "Creating FileSystem\n" );
	engineDesc.device.pAudio = new wv::AudioDevice( nullptr );
	wv::Debug::Print( "Created FileSystem\n" );

	// create modules
	wv::Debug::Print( "Creating FileSystem\n" );
	wv::cFileSystem* fileSystem = new wv::cFileSystem();
	fileSystem->addDirectory( L"res/" );
	fileSystem->addDirectory( L"res/materials/" );
	fileSystem->addDirectory( L"res/meshes/" );
	fileSystem->addDirectory( L"res/shaders/" );
	fileSystem->addDirectory( L"res/textures/" );
	engineDesc.systems.pFileSystem = fileSystem;
	wv::Debug::Print( "Created FileSystem\n" );
	wv::Debug::Print( "Creating ShaderRegistry\n" );
	engineDesc.systems.pShaderRegistry = new wv::cShaderRegistry( engineDesc.systems.pFileSystem, graphicsDevice );
	wv::Debug::Print( "Created ShaderRegistry\n" );

	// setup application state and scenes
	wv::cApplicationState* appState = new wv::cApplicationState();
	engineDesc.pApplicationState = appState;

	wv::cSceneRoot* scene = setupScene( fileSystem );
	appState->addScene( scene );

	// create engine
	m_pEngine = new wv::cEngine( &engineDesc );

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

wv::iSceneObject* parseSceneObject( nlohmann::json& _js )
{
	wv::iSceneObject* obj = nullptr;
	std::string objTypeName = _js[ "type" ]; // ew

	if( objTypeName == "Rigidbody" )
	{
		obj = wv::fromJson<wv::cRigidbody>( _js );
	}

	for( auto& childJson : _js[ "children" ] )
		obj->addChild( parseSceneObject( childJson ) );

	return obj;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cSceneRoot* cSandbox::setupScene( wv::cFileSystem* _pFileSystem )
{
	
	std::string src = _pFileSystem->loadString( "res/scenes/testScene.json" );
	nlohmann::json js = nlohmann::json::parse( src );
	
	wv::cSceneRoot* scene = new wv::cSceneRoot( js[ "name" ] );
	
	for( auto& objJson : js[ "scene" ] )
		scene->addChild( parseSceneObject( objJson ) );
	
	scene->addChild( new cTentacleSettingWindowObject( wv::cEngine::getUniqueUUID(), "tentacleSettingsWindow" ) );

	const int numSegments = 30;
	const float tentacleLength = 25.0f;
	const float tapre = 0.9f;
	const float segmentLength = tentacleLength / (float)numSegments;

	cTentacleSectionObject* section = nullptr;
	wv::iSceneObject* parent = scene;
	
	// tentacle
	for( int i = 0; i < numSegments; i++ )
	{
		section = new cTentacleSectionObject( wv::cEngine::getUniqueUUID(), "section", segmentLength );

		section->m_transform.setPosition( { 0.0f, segmentLength, 0.0f } );
		section->m_transform.setScale( { tapre } );
		
		parent->addChild( section );
		parent = section;
	}

	//for( int i = 0; i <numSegments; i++ )
	//{
	//	wv::sPhysicsBoxDesc* boxDesc = new wv::sPhysicsBoxDesc();
	//	boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
	//	boxDesc->halfExtent = { 0.5f, 0.5f, 0.5f };
	//	
	//	wv::cRigidbody* rb = new wv::cRigidbody( wv::cEngine::getUniqueUUID(), "rb", nullptr, boxDesc );
	//	rb->m_transform.setPosition( { 0.0f, ( float )i, 0.0f } );
	//	scene->addChild( rb );
	//}

	// skybox has to be added last
	/// TODO: fix that
	scene->addChild( new wv::cSkyboxObject( wv::cEngine::getUniqueUUID(), "Skybox" ) );

	return scene;
}

