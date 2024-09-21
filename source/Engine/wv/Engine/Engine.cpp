#include "Engine.h"

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <wv/Material/Material.h>
#include <wv/Texture/Texture.h>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/AudioDevice.h>

#include <wv/Memory/FileSystem.h>
#include <wv/Memory/ModelParser.h>

#include <wv/Physics/PhysicsEngine.h>
#include <wv/Primitive/Mesh.h>

#include <wv/RenderTarget/RenderTarget.h>
#include <wv/RenderTarget/IntermediateRenderTargetHandler.h>
#include <wv/Resource/ResourceRegistry.h>

#include <wv/Engine/EngineReflect.h>

#include <wv/Engine/ApplicationState.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>

#ifdef WV_SUPPORT_SDL2
#include <SDL2/SDL_keycode.h>
#endif

#include <type_traits>
#include <random>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif // EMSCRIPTEN

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif // WV_SUPPORT_IMGUI

#ifdef WV_SUPPORT_GLFW
#include <wv/Device/DeviceContext/GLFW/GLFWDeviceContext.h>
#endif // WV_SUPPORT_GLFW

///////////////////////////////////////////////////////////////////////////////////////

wv::cEngine* wv::cEngine::s_pInstance = nullptr;

///////////////////////////////////////////////////////////////////////////////////////

wv::cEngine::cEngine( EngineDesc* _desc )
{
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Engine\n" );

	if ( !_desc->pApplicationState )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "No application state provided. Cannot create application\n" );
		return;
	}

	s_pInstance = this;

	context  = _desc->device.pContext;
	graphics = _desc->device.pGraphics;

	m_pIRTHandler = _desc->pIRTHandler;
	
	if( m_pIRTHandler )
		m_pIRTHandler->create( graphics );

	m_pScreenRenderTarget = new RenderTarget();
	m_pScreenRenderTarget->width = _desc->windowWidth;
	m_pScreenRenderTarget->height = _desc->windowHeight;
	m_pScreenRenderTarget->fbHandle = 0;
	
	m_pApplicationState = _desc->pApplicationState;

	/// TODO: move to descriptor
	m_pPhysicsEngine = new cJoltPhysicsEngine();
	m_pPhysicsEngine->init();
	

	m_pFileSystem = _desc->systems.pFileSystem;
	m_pResourceRegistry = new cResourceRegistry( m_pFileSystem, graphics );
	m_pResourceRegistry->initializeEmbeded();

	graphics->initEmbeds();

	/* 
	 * create deferred rendering objects
	 * this should be configurable
	 */

#ifndef WV_PLATFORM_PSVITA // use forward rendering on vita
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Deferred Resources\n" );
	{ 
		m_deferredPipeline = new cProgramPipeline( "deferred" );
		m_deferredPipeline->load( m_pFileSystem, graphics );
		
		createScreenQuad();
		createGBuffer();
	}
#endif
	
	graphics->setRenderTarget( m_pScreenRenderTarget );
	graphics->setClearColor( wv::Color::Black );

	initImgui();

	Debug::Print( Debug::WV_PRINT_WARN, "TODO: Create AudioDeviceDesc\n" );
	Debug::Print( Debug::WV_PRINT_DEBUG, "Initializing Debug Draw\n" );
	Debug::Draw::Internal::initDebugDraw( graphics, m_pResourceRegistry );

	Debug::Print( "Created Engine\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cEngine* wv::cEngine::get()
{
	return s_pInstance;
}

wv::UUID wv::cEngine::getUniqueUUID()
{
#ifdef WV_PLATFORM_WINDOWS
	std::random_device rd;
	std::mt19937 gen( rd() );

	std::uniform_int_distribution<unsigned long long> dis(
		std::numeric_limits<std::uint64_t>::min(),
		std::numeric_limits<std::uint64_t>::max()
	);
	return dis( gen );
#else
	return 0;
#endif
}

wv::Handle wv::cEngine::getUniqueHandle()
{
#ifdef WV_PLATFORM_WINDOWS
	std::random_device rd;
	std::mt19937 gen( rd() );

	std::uniform_int_distribution<uint32_t> dis(
		std::numeric_limits<std::uint32_t>::min(),
		std::numeric_limits<std::uint32_t>::max()
	);

	return dis( gen );
#else
	return 0;
#endif
}


///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::onResize( int _width, int _height )
{
	context->onResize( _width, _height );
	recreateScreenRenderTarget( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::onMouseEvent( MouseEvent _event )
{
	m_mousePosition = _event.position;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::onInputEvent( sInputEvent _event )
{
	if ( _event.buttondown )
	{
		switch ( _event.key )
		{
		case 'R':         m_pApplicationState->reloadScene(); break;
		}

		switch ( context->getContextAPI() )
		{
		case WV_DEVICE_CONTEXT_API_SDL:
			if ( _event.key == SDLK_ESCAPE ) context->close();
			break;
		case WV_DEVICE_CONTEXT_API_GLFW:
			if ( _event.key == GLFW_KEY_ESCAPE ) context->close();
			break;
		}
	}
}

void wv::cEngine::setSize( int _width, int _height, bool _notify )
{
	context->setSize( _width, _height );
	
	if( _notify )
		onResize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

#ifdef EMSCRIPTEN
void emscriptenMainLoop() { wv::cEngine::get()->tick(); }
#endif

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector2i wv::cEngine::getViewportSize()
{
	if( m_pIRTHandler )
	{
		if( m_pIRTHandler->m_pRenderTarget )
			return { m_pIRTHandler->m_pRenderTarget->width, m_pIRTHandler->m_pRenderTarget->height };
		else
			return { 1,1 };
	}
	
	return { context->getWidth(), context->getHeight() };
}

void wv::cEngine::run()
{
	// Subscribe to user input event
	subscribeMouseEvents();
	subscribeInputEvent();
	
	/*
	 * Cameras have to be made after the event is subscribed to
	 * to get the correct event callback order
	 * 
	 * This is because events are called in the order that they 
	 * were subscribed in
	 */

	orbitCamera = new OrbitCamera( iCamera::WV_CAMERA_TYPE_PERSPECTIVE );
	freeflightCamera = new FreeflightCamera( iCamera::WV_CAMERA_TYPE_PERSPECTIVE );
	orbitCamera->onCreate();
	freeflightCamera->onCreate();

	freeflightCamera->getTransform().setPosition( { 0.0f, 0.0f, 20.0f } );

	currentCamera = freeflightCamera;
	
	m_pApplicationState->onCreate();
	m_pApplicationState->switchToScene( 0 ); // default scene

	// while m_applicationState->isLoading() { doloadingstuff }
	
#ifdef EMSCRIPTEN
	emscripten_set_main_loop( []{ wv::cEngine::get()->tick(); }, 0, 1);
#else
	while ( context->isAlive() )
		tick();
#endif

	Debug::Print( Debug::WV_PRINT_DEBUG, "Quitting...\n" );
	
	m_pApplicationState->onDestroy();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::terminate()
{
	currentCamera = nullptr;
	
	m_pPhysicsEngine->terminate();

	delete orbitCamera;
	delete freeflightCamera;
	orbitCamera = nullptr;
	freeflightCamera = nullptr;

	graphics->destroyMesh( m_screenQuad );
	graphics->destroyRenderTarget( &m_gbuffer );

	// destroy modules
	Debug::Draw::Internal::deinitDebugDraw( graphics );
	delete m_pFileSystem;

	context->terminate();
	graphics->terminate();
	
	delete context;
	delete graphics;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::tick()
{
	double dt = context->getDeltaTime();
	
#ifdef WV_PLATFORM_WASM
	// user needs to interact with tab before 
	// audio device can be initalized
	if ( !audio->isEnabled() )
		audio->initialize();
#endif

	/// ------------------ update ------------------ ///

	context->pollEvents();
	
#ifdef WV_PLATFORM_WINDOWS
	// refresh fps display only on windows
	{
		double fps = 1.0 / dt;
		if ( fps > m_maxFps )
			m_maxFps = fps;

		m_fpsCache[ m_fpsCacheCounter ] = fps;
		m_fpsCacheCounter++;
		if ( m_fpsCacheCounter >= FPS_CACHE_NUM )
		{
			m_fpsCacheCounter = 0;
			for ( int i = 0; i < FPS_CACHE_NUM; i++ )
				m_averageFps += m_fpsCache[ i ];
			m_averageFps /= (double)FPS_CACHE_NUM;

			std::string title = "FPS: " + std::to_string( (int)m_averageFps ) + "   MAX: " + std::to_string( (int)m_maxFps );
			context->setTitle( title.c_str() );
		}
	}
#endif

#ifdef WV_SUPPORT_JOLT_PHYSICS
	m_pPhysicsEngine->update( dt );
#endif

	// update modules

	m_pApplicationState->update( dt );
	
	currentCamera->update( dt );

	/// ------------------ render ------------------ ///
	

#ifndef WV_PLATFORM_PSVITA
	if( !m_gbuffer )
		return;
#endif

	if ( m_pScreenRenderTarget->width == 0 || m_pScreenRenderTarget->height == 0 )
		return;

#ifdef WV_PLATFORM_PSVITA
	graphics->setRenderTarget( nullptr );
#else
	graphics->setRenderTarget( m_gbuffer );
#endif

	graphics->beginRender();
	graphics->clearRenderTarget( true, true );

#ifdef WV_SUPPORT_IMGUI
	context->newImGuiFrame();
	ImGui::DockSpaceOverViewport( 0, 0, ImGuiDockNodeFlags_PassthruCentralNode );
#endif // WV_SUPPORT_IMGUI
	
	if( m_drawWireframe ) graphics->setFillMode( WV_FILL_MODE_WIREFRAME );

	m_pApplicationState->draw( context, graphics );
	m_pResourceRegistry->drawMeshInstances();

	if( m_drawWireframe ) graphics->setFillMode( WV_FILL_MODE_SOLID );

#ifdef WV_DEBUG
	Debug::Draw::Internal::drawDebug( graphics );
#endif

#ifndef WV_PLATFORM_PSVITA
	if( m_pIRTHandler )
	{
		if( m_pIRTHandler->m_pRenderTarget )
			graphics->setRenderTarget( m_pIRTHandler->m_pRenderTarget );
	}
	else
		graphics->setRenderTarget( m_pScreenRenderTarget );
	
	graphics->clearRenderTarget( true, true );

	// bind gbuffer textures to deferred pass
	for ( int i = 0; i < m_gbuffer->numTextures; i++ )
		graphics->bindTextureToSlot( m_gbuffer->textures[ i ], i );

	// render screen quad with deferred shader
	m_deferredPipeline->use( graphics );
	graphics->draw( m_screenQuad );
	
	if( m_pIRTHandler )
	{
		graphics->setRenderTarget( m_pScreenRenderTarget );
		graphics->clearRenderTarget( true, true );
		m_pIRTHandler->draw( graphics );
	}
#endif

#ifdef WV_SUPPORT_IMGUI
	context->renderImGui();
#endif

	graphics->endRender();

	context->swapBuffers();

#ifndef WV_PLATFORM_PSVITA
	if( m_pIRTHandler )
	{
		if( m_pIRTHandler->shouldRecreate() )
		{
			m_pIRTHandler->destroy();
			m_pIRTHandler->create( graphics );

			if( m_pIRTHandler->m_pRenderTarget )
				recreateScreenRenderTarget( m_pIRTHandler->m_pRenderTarget->width, m_pIRTHandler->m_pRenderTarget->height );
		}
	}
#endif

}

void wv::cEngine::quit()
{
	shutdownImgui();
	context->close();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::initImgui()
{
#ifdef WV_SUPPORT_IMGUI
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Initializing ImGui\n" );

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	context->initImGui();
#else
	Debug::Print( Debug::WV_PRINT_WARN, "ImGui not supported on this platform\n" );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::shutdownImgui()
{
#ifdef WV_SUPPORT_IMGUI
	context->terminateImGui();
	ImGui::DestroyContext();
#endif // WV_SUPPORT_IMGUI
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::createScreenQuad()
{
	//wv::sVertexAttribute elements[] = {
	//	{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 },
	//	{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }
	//};

	/// TODO: allow for unique layouts
	wv::sVertexAttribute elements[] = {
			{ "a_Pos",       3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ "a_Normal",    3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ "a_Tangent",   3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ "a_Color",     4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ "a_TexCoord0", 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};

	wv::sVertexLayout layout;
	layout.elements = elements;
	layout.numElements = 5;

	std::vector<Vertex> vertices;
	vertices.push_back( Vertex{ {  3.0f, -1.0f, 0.5f }, {}, {}, {}, { 2.0f, 0.0f } } );
	vertices.push_back( Vertex{ { -1.0f,  3.0f, 0.5f }, {}, {}, {}, { 0.0f, 2.0f } } );
	vertices.push_back( Vertex{ { -1.0f, -1.0f, 0.5f }, {}, {}, {}, { 0.0f, 0.0f } } );
	
	std::vector<uint32_t> indices;
	indices.push_back( 0 );
	indices.push_back( 1 );
	indices.push_back( 2 );
	wv::sMeshDesc prDesc;
	{
		prDesc.layout = layout;

		prDesc.vertices  = vertices.data();
		prDesc.sizeVertices = vertices.size() * sizeof( Vertex );

		prDesc.pIndices32 = indices.data();
		prDesc.numIndices = indices.size();
	}

	m_screenQuad = graphics->createMesh( &prDesc );
	
	if( m_screenQuad )
		m_screenQuad->transform.update( nullptr );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::createGBuffer()
{
	Vector2i size = getViewportSize();

	RenderTargetDesc rtDesc;
	rtDesc.width  = size.x;
	rtDesc.height = size.y;
#ifdef WV_PLATFORM_WINDOWS
	TextureDesc texDescs[] = {
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_BYTE },
	#ifdef EMSCRIPTEN
		// WebGL doesn't seem to support FLOAT R, RG, or RGB
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_FLOAT },
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_FLOAT },
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_FLOAT }
	#else
		{ wv::WV_TEXTURE_CHANNELS_RGB, wv::WV_TEXTURE_FORMAT_FLOAT },
		{ wv::WV_TEXTURE_CHANNELS_RGB, wv::WV_TEXTURE_FORMAT_FLOAT },
		{ wv::WV_TEXTURE_CHANNELS_RG,  wv::WV_TEXTURE_FORMAT_FLOAT }
	#endif
	};
	rtDesc.pTextureDescs = texDescs;
	rtDesc.numTextures = 4;
#endif

	uint32_t buffer = graphics->getCommandBuffer();
	graphics->bufferCommand( buffer, WV_GPUTASK_CREATE_RENDERTARGET, &m_gbuffer, &rtDesc );
	graphics->submitCommandBuffer( buffer );
	graphics->executeCommandBuffer( buffer );


}

void wv::cEngine::recreateScreenRenderTarget( int _width, int _height )
{
	graphics->onResize( _width, _height );

	// recreate render target
	m_pScreenRenderTarget->width = _width;
	m_pScreenRenderTarget->height = _height;
	
	if( m_gbuffer )
		graphics->destroyRenderTarget( &m_gbuffer );

	if( _width == 0 || _height == 0 )
		return;

	createGBuffer();
}

