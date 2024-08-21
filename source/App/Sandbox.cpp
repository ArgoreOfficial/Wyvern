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


void cSandbox::run( void )
{
	m_pEngine->run();
}


void cSandbox::destroy( void )
{
	m_pEngine->terminate();
}

struct node
{
	double prob, range_from, range_to;
};

double encoding( std::string s, std::unordered_map<char, node> arr )
{
	//cout << "\nEncoding\n";
	double low_v = 0.0, high_v = 1.0, diff = 1.0;
	//cout << "Symbol\tLow_v\tHigh_v\tdiff\n";
	for( int i = 0; i < s.size(); i++ )
	{
		high_v = low_v + diff * arr[ s[ i ] ].range_to;
		low_v = low_v + diff * arr[ s[ i ] ].range_from;
		diff = high_v - low_v;
		//cout << s[ i ] << "\t" << low_v << "\t" << high_v << "\t" << diff << endl;
	}
	return low_v;
}
std::string decoding( std::unordered_map<char, node> arr, double code_word, int len )
{
	char ch;
	std::string text = "";
	int j = 0;
	std::unordered_map<char, node>::iterator it;
	while( j < len )
	{
		for( it = arr.begin(); it != arr.end(); it++ )
		{
			char i = ( *it ).first;
			if( arr[ i ].range_from <= code_word && code_word < arr[ i ].range_to )
			{
				ch = i;
				code_word = ( code_word - arr[ i ].range_from ) / ( arr[ i ].range_to - arr[ i ].range_from );
				break;
			}
		}
		text += ch;
		j++;
	}
	return text;
}

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

// https://github.com/dmitrykravchenko2018/arithmetic_coding/blob/master/encode.cpp
void dothing()
{
	std::string encodeString = "this is an epic sentence with many chars";
	std::unordered_map<char, node> arr;

	double rangeFrom = 0;
	for( size_t i = 0; i < encodeString.size(); i++ )
	{
		char letter = encodeString[ i ];

		if( arr.contains( letter ) )
			continue;
		
		arr[ letter ] = {};
		arr[ letter ].prob = ( double )std::count( encodeString.begin(), encodeString.end(), letter ) / (double)(encodeString.size());
		arr[ letter ].range_from = rangeFrom;
		arr[ letter ].range_to = rangeFrom + arr[ letter ].prob;
		rangeFrom = arr[ letter ].range_to;
	}

	double codeword = encoding( encodeString, arr );

	printf( "\n%f\n", codeword );
	printf( "%s\n", decoding( arr, codeword, encodeString.size()).c_str() );
}

wv::cSceneRoot* cSandbox::setupScene( wv::cFileSystem* _pFileSystem )
{
	
	dothing();

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
	
	for( int i = 0; i < numSegments; i++ )
	{
		section = new cTentacleSectionObject( wv::cEngine::getUniqueUUID(), "section", segmentLength );

		section->m_transform.setPosition( { 0.0f, segmentLength, 0.0f } );
		section->m_transform.setScale( { tapre } );
		
		parent->addChild( section );
		parent = section;
	}

	// floor
	{
		wv::sPhysicsBoxDesc* boxDesc = new wv::sPhysicsBoxDesc();
		boxDesc->halfExtent = { 100.0f, 1.0f, 100.0f };

		wv::cRigidbody* floor = new wv::cRigidbody( wv::cEngine::getUniqueUUID(), "rb", nullptr, boxDesc );
		floor->m_transform.position.y = -7.0f;
		floor->m_transform.scale = { 200.0f, 2.0f, 200.0f };
		scene->addChild( floor );
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

