#include "Engine.h"

#include <glad/glad.h>

#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Materials/MaterialRegistry.h>
#include <wv/Assets/Texture.h>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/AudioDevice.h>

#include <wv/Memory/MemoryDevice.h>
#include <wv/Memory/ModelParser.h>

#include <wv/Physics/PhysicsEngine.h>
#include <wv/Primitive/Mesh.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <wv/Scene/SceneRoot.h>
#include <wv/Scene/Model.h>
#include <wv/Scene/Rigidbody.h>
#include <wv/Scene/Skybox.h>

#include <wv/Shader/ShaderRegistry.h>
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
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#endif // WV_SUPPORT_IMGUI

#ifdef WV_SUPPORT_GLFW
#include <wv/Device/DeviceContext/GLFW/GLFWDeviceContext.h>
#endif // WV_SUPPORT_GLFW

///////////////////////////////////////////////////////////////////////////////////////

wv::cEngine::cEngine( EngineDesc* _desc )
{
	if ( !_desc->pApplicationState )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "No application state provided. Cannot create application\n" );
		return;
	}

	s_pInstance = this;

	context  = _desc->device.pContext;
	graphics = _desc->device.pGraphics;

	m_pFileSystem       = _desc->systems.pFileSystem;
	m_pShaderRegistry   = _desc->systems.pShaderRegistry;
	m_pMaterialRegistry = new cMaterialRegistry( m_pFileSystem, graphics, m_pShaderRegistry );

	m_defaultRenderTarget = new RenderTarget();
	m_defaultRenderTarget->width  = _desc->windowWidth;
	m_defaultRenderTarget->height = _desc->windowHeight;
	m_defaultRenderTarget->fbHandle = 0;

	m_pApplicationState = _desc->pApplicationState;

	/// TODO: move to descriptor
	m_pPhysicsEngine = new cJoltPhysicsEngine();
	m_pPhysicsEngine->init();

	/* 
	 * create deferred rendering objects
	 * this should be configurable
	 */
	{ 
		m_deferredProgram = m_pShaderRegistry->loadProgramFromWShader( "res/shaders/deferred.wshader" );
		createScreenQuad();
		createGBuffer();
	}
	
	graphics->setRenderTarget( m_defaultRenderTarget );
	graphics->setClearColor( wv::Color::Black );

	initImgui();

	Debug::Print( Debug::WV_PRINT_WARN, "TODO: Create AudioDeviceDesc\n" );

	Debug::Draw::Internal::initDebugDraw( graphics, m_pMaterialRegistry );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cEngine* wv::cEngine::get()
{
	return s_pInstance;
}

wv::UUID wv::cEngine::getUniqueUUID()
{
	std::random_device rd;
	std::mt19937 gen( rd() );

	std::uniform_int_distribution<unsigned long long> dis(
		std::numeric_limits<std::uint64_t>::min(),
		std::numeric_limits<std::uint64_t>::max()
	);

	return dis( gen );
}

wv::Handle wv::cEngine::getUniqueHandle()
{
	std::random_device rd;
	std::mt19937 gen( rd() );

	std::uniform_int_distribution<uint32_t> dis(
		std::numeric_limits<std::uint32_t>::min(),
		std::numeric_limits<std::uint32_t>::max()
	);

	return dis( gen );
}


///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::onResize( int _width, int _height )
{
	context->onResize( _width, _height );
	graphics->onResize( _width, _height );

	// recreate render target
	m_defaultRenderTarget->width = _width;
	m_defaultRenderTarget->height = _height;
	graphics->setRenderTarget( m_defaultRenderTarget );

	graphics->destroyRenderTarget( &m_gbuffer );
	
	if ( _width == 0 || _height == 0 )
		return;
	
	createGBuffer();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::onMouseEvent( MouseEvent _event )
{
	m_mousePosition = _event.position;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::onInputEvent( InputEvent _event )
{
	if( _event.buttondown && _event.key == 'R' )
			m_pApplicationState->reloadScene();
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
	orbitCamera = new OrbitCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	freeflightCamera = new FreeflightCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
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

	graphics->destroyMesh( &m_screenQuad );
	graphics->destroyRenderTarget( &m_gbuffer );
	m_pShaderRegistry->unloadShaderProgram( m_deferredProgram );

	// destroy modules
	Debug::Draw::Internal::deinitDebugDraw( graphics );
	delete m_pFileSystem;
	delete m_pShaderRegistry;

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
	
	// refresh fps display
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

	m_pPhysicsEngine->update( dt );

	// update modules
	m_pShaderRegistry->update();
	m_pMaterialRegistry->update();

	m_pApplicationState->update( dt );
	
	currentCamera->update( dt );

	/// ------------------ render ------------------ ///
	
	if ( !m_gbuffer || m_defaultRenderTarget->width == 0 || m_defaultRenderTarget->height == 0 )
		return;

	graphics->setRenderTarget( m_gbuffer );
	graphics->clearRenderTarget( true, true );

#ifdef WV_SUPPORT_IMGUI
	/// TODO: move
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport( 0, 0, ImGuiDockNodeFlags_PassthruCentralNode );
#endif // WV_SUPPORT_IMGUI

	m_pApplicationState->draw( context, graphics );

	Debug::Draw::Internal::drawDebug( graphics );

	// normal back buffer
	graphics->setRenderTarget( m_defaultRenderTarget );
	graphics->clearRenderTarget( true, true );

	// bind gbuffer textures to deferred pass
	for ( int i = 0; i < m_gbuffer->numTextures; i++ )
		graphics->bindTextureToSlot( m_gbuffer->textures[ i ], i );

	// render screen quad with deferred shader
	graphics->useProgram( m_deferredProgram );
	graphics->draw( m_screenQuad );
	
#ifdef WV_SUPPORT_IMGUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
#endif // WV_SUPPORT_IMGUI

	context->swapBuffers();
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	context->initImGui();

	switch( context->getGraphicsAPI() )
	{
	case WV_GRAPHICS_API_OPENGL: ImGui_ImplOpenGL3_Init(); break;
	}
#else
	Debug::Print( Debug::WV_PRINT_WARN, "ImGui not supported\n" );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::shutdownImgui()
{
#ifdef WV_SUPPORT_IMGUI
	switch( context->getGraphicsAPI() )
	{
	case WV_GRAPHICS_API_OPENGL: ImGui_ImplOpenGL3_Shutdown(); break;
	}

	context->terminateImGui();

	ImGui::DestroyContext();
#endif // WV_SUPPORT_IMGUI
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::createScreenQuad()
{
	//wv::InputLayoutElement elements[] = {
	//	{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 },
	//	{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }
	//};

	/// TODO: allow for unique layouts
	wv::InputLayoutElement elements[] = {
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ 4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};

	wv::InputLayout layout;
	layout.elements = elements;
	layout.numElements = 5;

	std::vector<Vertex> vertices;
	vertices.push_back( Vertex{ {  3.0f, -1.0f, 0.5f }, {}, {}, {}, { 2.0f, 0.0f } } );
	vertices.push_back( Vertex{ { -1.0f,  3.0f, 0.5f }, {}, {}, {}, { 0.0f, 2.0f } } );
	vertices.push_back( Vertex{ { -1.0f, -1.0f, 0.5f }, {}, {}, {}, { 0.0f, 0.0f } } );
	
	std::vector<uint32_t> indices = { 0, 1, 2 };

	wv::PrimitiveDesc prDesc;
	{
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;

		prDesc.vertices = vertices;
		prDesc.indices  = indices;
	}

	MeshDesc meshDesc;
	m_screenQuad = graphics->createMesh( &meshDesc );
	graphics->createPrimitive( &prDesc, m_screenQuad );
	m_screenQuad->transform.update();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::createGBuffer()
{
	RenderTargetDesc rtDesc;
	rtDesc.width  = context->getWidth();
	rtDesc.height = context->getHeight();
	
	//rtDesc.width  = 640;
	//rtDesc.height = 480;

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
	rtDesc.textureDescs = texDescs;
	rtDesc.numTextures = 4;

	m_gbuffer = graphics->createRenderTarget( &rtDesc );
}
