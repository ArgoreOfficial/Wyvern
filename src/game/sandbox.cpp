#include "Sandbox.h"

#include <wv/engine.h>

#include <wv/camera/camera.h>
#include <wv/device/audio_device.h>
#include <wv/device/device_context.h>
#include <wv/debug/draw.h>

#include <wv/filesystem/file_system.h>
#include <wv/graphics/graphics_device.h>

#include <wv/shader/shader_resource.h>
#include <wv/renderer/mesh_renderer.h>
#include <wv/physics/physics_engine.h>

#include <wv/memory/memory.h>
#include <wv/resource/resource_registry.h>
#include <noapi/noapi_file_system.h>

#include <wv/scene/scene.h>

#include "scene/demo_window.h"
#include "state/editor_app_state.h"
#include "state/runtime_app_state.h"

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif // WV_SUPPORT_IMGUI

#ifdef WV_PLATFORM_PSVITA
#include <wv/Platform/PSVita.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

// REFLECT_CLASS( DemoWindow );

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
#ifdef WV_PACKAGE
	wv::IAppState* appState = WV_NEW( RuntimeAppState );
#else
	wv::IAppState* appState = WV_NEW( EditorAppState );
#endif

	engineDesc.pAppState = appState;

	// create engine
	m_pEngine = WV_NEW( wv::Engine, &engineDesc );

	// load scenes
	/// TODO: change to appState->addScene( "scenes/playground.json" );
	/// OR: load scene as template, might allow for easier reload of original states
	///		loading:   load file -> parse template -> create Scene from template
	///		reloading: destroy Scene -> create Scene from saved template
	/// preloads all templates for quick scene switching
	wv::Scene* scene = appState->loadScene( fileSystem, "scenes/playground.json" );
	wv::Scene* scene2 = appState->loadScene( fileSystem, "scenes/defaultScene.json" );
	appState->addScene( scene ); // the engine will load into scene 0 by default
	appState->addScene( scene2 );
	
	// set fog params 
	/// TODO: move to scene
	m_pEngine->m_fogParams.colorDensity = { 0.85f, 0.f, 0.f, 0.15f };
	m_pEngine->m_fogParams.isEnabled = 0;

	if( !wv::Engine::get() )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_FATAL, "Couldn't create Engine\n" );
		return false;
	}

	initImgui();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void Sandbox::run( void )
{
	// Subscribe to user input event
	m_pEngine->m_inputListener.hook();
	m_pEngine->m_mouseListener.hook();

	size_t embeddedResources = m_pEngine->m_pResourceRegistry->getNumLoadedResources();

	m_pEngine->m_pAppState->switchToScene( 0 ); // default scene

	// wait for load to be done
	m_pEngine->m_pResourceRegistry->waitForFence();

#ifdef EMSCRIPTEN
	emscripten_set_main_loop( [] { wv::Engine::get()->tick(); }, 0, 1 );
#else

	int timeToDeath = 5;
	int deathCounter = 0;
	double deathTimer = 0.0;
	while ( m_pEngine->context->isAlive() )
	{
	#ifndef WV_PACKAGE
		m_pEngine->m_pThreadProfiler->begin();
	#endif

		tick();

	#ifndef WV_PACKAGE
		m_pEngine->m_pThreadProfiler->end();

		/// TODO: thread profiler post-tick event

		m_pEngine->m_pThreadProfiler->reset();
	#endif

		// automatic shutdown if the context is NONE
		if ( m_pEngine->context->getContextAPI() == wv::WV_DEVICE_CONTEXT_API_NONE )
		{
			double t = m_pEngine->context->getTime();

			deathTimer = t - static_cast<double>( deathCounter );
			if ( deathTimer > 1.0 )
			{
				wv::Debug::Print( "AWESOME SAUCE AND COOL: %i\n", timeToDeath - deathCounter );
				deathCounter++;
			}

			if ( deathCounter > timeToDeath )
				m_pEngine->context->close();
		}
	}

#endif

	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Quitting...\n" );

	m_pEngine->m_pAppState->onExit();
	m_pEngine->m_pAppState->onDestruct();

	// wait for unload to be done
	m_pEngine->m_pResourceRegistry->waitForFence();
}

///////////////////////////////////////////////////////////////////////////////////////

void Sandbox::destroy( void )
{
	shutdownImgui();

	m_pEngine->terminate();
	WV_FREE( m_pEngine );
	m_pEngine = nullptr;
} // destroy

///////////////////////////////////////////////////////////////////////////////////////

void Sandbox::initImgui()
{
	{
	#ifdef WV_SUPPORT_IMGUI
		wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Initializing ImGui\n" );

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		m_pEngine->context->initImGui();

		// Steam style from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6000000238418579f;
		style.WindowPadding = ImVec2( 8.0f, 8.0f );
		style.WindowRounding = 0.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2( 32.0f, 32.0f );
		style.WindowTitleAlign = ImVec2( 0.0f, 0.5f );
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2( 4.0f, 3.0f );
		style.FrameRounding = 0.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2( 8.0f, 4.0f );
		style.ItemInnerSpacing = ImVec2( 4.0f, 4.0f );
		style.CellPadding = ImVec2( 4.0f, 2.0f );
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.TabBorderSize = 1.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2( 0.5f, 0.5f );
		style.SelectableTextAlign = ImVec2( 0.0f, 0.699999988079071f );

		style.Colors[ ImGuiCol_Text ] = ImVec4( 1.0f, 0.9999899864196777f, 0.9999899864196777f, 1.0f );
		style.Colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f );
		style.Colors[ ImGuiCol_WindowBg ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_ChildBg ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_PopupBg ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_Border ] = ImVec4( 0.501960813999176f, 0.501960813999176f, 0.501960813999176f, 1.0f );
		style.Colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.1568627506494522f, 0.1803921610116959f, 0.1333333402872086f, 1.0f );

		style.Colors[ ImGuiCol_DockingPreview ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );
		style.Colors[ ImGuiCol_DockingEmptyBg ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );

		style.Colors[ ImGuiCol_FrameBg ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );
		style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.2470588237047195f, 0.2745098173618317f, 0.2196078449487686f, 1.0f );
		style.Colors[ ImGuiCol_TitleBg ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 1.0f, 1.0f, 1.0f, 0.5099999904632568f );
		style.Colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.686274528503418f, 0.686274528503418f, 0.686274528503418f, 0.800000011920929f );
		style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.4862745106220245f, 0.4862745106220245f, 0.4862745106220245f, 0.800000011920929f );
		style.Colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.4862745106220245f, 0.4862745106220245f, 0.4862745106220245f, 1.0f );
		style.Colors[ ImGuiCol_CheckMark ] = ImVec4( 0.9999899864196777f, 0.9999945759773254f, 1.0f, 1.0f );
		style.Colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.772549033164978f, 0.7137255072593689f, 0.3137255012989044f, 1.0f );
		style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.772549033164978f, 0.7137255072593689f, 0.3137255012989044f, 1.0f );
		style.Colors[ ImGuiCol_Button ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );
		style.Colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_Header ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );
		style.Colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_Separator ] = ImVec4( 1.0f, 0.0f, 0.0f, 0.6200000047683716f );
		style.Colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );
		style.Colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.529411792755127f, 0.5647059082984924f, 0.4980392158031464f, 1.0f );
		style.Colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );
		style.Colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_Tab ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_TabHovered ] = ImVec4( 0.5882353186607361f, 0.5372549295425415f, 0.1882352977991104f, 1.0f );
		style.Colors[ ImGuiCol_TabActive ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.2980392277240753f, 0.3450980484485626f, 0.2666666805744171f, 1.0f );
		style.Colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_PlotLines ] = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
		style.Colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 0.5803921818733215f, 0.5372549295425415f, 0.196078434586525f, 1.0f );
		style.Colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.7686274647712708f, 0.7098039388656616f, 0.3137255012989044f, 1.0f );
		style.Colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 0.5803921818733215f, 0.5372549295425415f, 0.196078434586525f, 1.0f );
		style.Colors[ ImGuiCol_TableHeaderBg ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 1.0f );
		style.Colors[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.1568627506494522f, 0.1803921610116959f, 0.1333333402872086f, 1.0f );
		style.Colors[ ImGuiCol_TableBorderLight ] = ImVec4( 0.1568627506494522f, 0.1803921610116959f, 0.1333333402872086f, 1.0f );
		style.Colors[ ImGuiCol_TableRowBg ] = ImVec4( 0.2431372553110123f, 0.2745098173618317f, 0.2156862765550613f, 0.3476395010948181f );
		style.Colors[ ImGuiCol_TableRowBgAlt ] = ImVec4( 1.0f, 0.9999899864196777f, 0.9999899864196777f, 0.07725322246551514f );
		style.Colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.7686274647712708f, 0.7098039388656616f, 0.3137255012989044f, 1.0f );
		style.Colors[ ImGuiCol_DragDropTarget ] = ImVec4( 0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.949999988079071f );
		style.Colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f );
		style.Colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.699999988079071f );
		style.Colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f );
		style.Colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3499999940395355f );
	#else
		Debug::Print( Debug::WV_PRINT_WARN, "ImGui not supported on this platform\n" );
	#endif
	}
}

void Sandbox::shutdownImgui()
{
#ifdef WV_SUPPORT_IMGUI
	m_pEngine->context->terminateImGui();
	ImGui::DestroyContext();
#endif // WV_SUPPORT_IMGUI
}

void Sandbox::tick()
{
	double dt = m_pEngine->context->getDeltaTime();

#ifdef WV_PLATFORM_WASM
	// user needs to interact with tab before 
	// audio device can be initalized
	if ( !audio->isEnabled() )
		audio->initialize();
#endif

	/// ------------------ update ------------------ ///

	m_pEngine->context->pollEvents();
	m_pEngine->handleInput();

#ifdef WV_PLATFORM_WINDOWS
	// refresh fps display only on windows
	refreshFPSDisplay( dt );
#endif // WV_PLATFORM_WINDOWS

#ifdef WV_SUPPORT_JOLT_PHYSICS
	wv::Fence* physicsFence = m_pEngine->m_pJobSystem->createFence();
	wv::Job::JobFunction_t fptr = []( void* _pUserData )
		{
			wv::Engine::get()->m_pPhysicsEngine->update( *(double*)_pUserData );
		};
	wv::Job* job = m_pEngine->m_pJobSystem->createJob( fptr, physicsFence, nullptr, &dt );
	m_pEngine->m_pJobSystem->submit( { job } );
#endif // WV_SUPPORT_JOLT_PHYSICS

	m_pEngine->m_pAppState->onUpdate( dt );
	m_pEngine->m_pAppState->currentCamera->update( dt );

#ifdef WV_SUPPORT_JOLT_PHYSICS
	m_pEngine->m_pJobSystem->waitAndDeleteFence( physicsFence );
#endif // WV_SUPPORT_JOLT_PHYSICS


	/// ------------------ render ------------------ ///	

#ifdef WV_PLATFORM_WINDOWS
	if ( !m_pEngine->m_gbuffer.is_valid() )
		return;
#endif

	{
		wv::RenderTarget& rt = m_pEngine->graphics->m_renderTargets.at( m_pEngine->m_screenRenderTarget );
		if ( rt.width == 0 || rt.height == 0 )
			return;
	}

#ifdef WV_PLATFORM_WINDOWS
	m_pEngine->graphics->cmdBeginRender( {}, m_pEngine->m_gbuffer );
#else
	m_pEngine->graphics->cmdBeginRender( {}, {} );
#endif

	m_pEngine->graphics->beginRender();

#ifdef WV_SUPPORT_IMGUI
	if ( m_pEngine->context->newImGuiFrame() )
	{
		ImGui::DockSpaceOverViewport( 0, 0, ImGuiDockNodeFlags_PassthruCentralNode );
	}
#endif // WV_SUPPORT_IMGUI

	if ( m_pEngine->m_pAppState->currentCamera->beginRender( m_pEngine->graphics, m_pEngine->m_drawWireframe ? wv::WV_FILL_MODE_WIREFRAME : wv::WV_FILL_MODE_SOLID ) )
	{
		m_pEngine->graphics->cmdClearColor( {}, 0.0f, 0.0f, 0.0f, 1.0f );
		m_pEngine->graphics->cmdClearDepthStencil( {}, 1.0, 0 );

		m_pEngine->m_pAppState->onDraw( m_pEngine->context, m_pEngine->graphics );
		m_pEngine->m_pMeshRenderer->render();

	#ifdef WV_DEBUG
		wv::Debug::Draw::Internal::drawDebug( m_pEngine->graphics );
	#endif
	}

#ifdef WV_PLATFORM_WINDOWS
	m_pEngine->graphics->cmdBeginRender( {}, m_pEngine->m_screenRenderTarget );

	m_pEngine->graphics->cmdClearColor( {}, 0.0f, 0.0f, 0.0f, 1.0f );
	m_pEngine->graphics->cmdClearDepthStencil( {}, 1.0, 0 );

	// bind gbuffer textures to deferred pass
	{
		wv::RenderTarget& rt = m_pEngine->graphics->m_renderTargets.at( m_pEngine->m_gbuffer );
		for ( int i = 0; i < rt.numTextures; i++ )
			m_pEngine->graphics->bindTextureToSlot( rt.pTextureIDs[ i ], i );
	}

	// render screen quad with deferred shader
	if ( m_pEngine->m_pDeferredShader->isComplete() && m_pEngine->m_pAppState->currentCamera->beginRender( m_pEngine->graphics, wv::WV_FILL_MODE_SOLID ) )
	{
		m_pEngine->m_pDeferredShader->bind( m_pEngine->graphics );

		wv::GPUBufferID UbFogParams  = m_pEngine->m_pDeferredShader->getShaderBuffer( "UbFogParams" );
		wv::GPUBufferID SbVerticesID = m_pEngine->m_pDeferredShader->getShaderBuffer( "SbVertices" );
		wv::GPUBuffer fogParamBuffer = m_pEngine->graphics->m_gpuBuffers.at( UbFogParams );

		m_pEngine->graphics->bufferSubData( UbFogParams, &m_pEngine->m_fogParams, sizeof( m_pEngine->m_fogParams ), 0 );
		m_pEngine->graphics->bindBufferIndex( UbFogParams, fogParamBuffer.bindingIndex.value );

		wv::Mesh screenQuad = m_pEngine->graphics->m_meshes.at( m_pEngine->m_screenQuad );
		{
			wv::GPUBuffer& SbVertices = m_pEngine->graphics->m_gpuBuffers.at( SbVerticesID );
			m_pEngine->graphics->bindBufferIndex( screenQuad.vertexBufferID, SbVertices.bindingIndex.value );
			m_pEngine->graphics->cmdBindIndexBuffer( {}, screenQuad.indexBufferID, 0, {} );
		}

		// GPUBuffer& ibuffer = graphics->m_gpuBuffers.at( screenQuad.indexBufferID );
		m_pEngine->graphics->cmdDrawIndexed( {}, screenQuad.numIndices, 1, 0, 0, 0 );
	}
#endif

#ifdef WV_SUPPORT_IMGUI
	m_pEngine->context->renderImGui();
#endif

	m_pEngine->graphics->endRender();

	m_pEngine->context->swapBuffers();
}

void Sandbox::refreshFPSDisplay( double _deltaTime )
{
	double fps = 1.0 / _deltaTime;
	if ( fps > m_maxFps )
		m_maxFps = fps;

	m_timeSinceFPSUpdate += _deltaTime;
	m_fpsAccumulator += fps;
	m_fpsCounter++;

	if ( m_timeSinceFPSUpdate >= m_fpsUpdateInterval )
	{
		m_averageFps = m_fpsAccumulator / (double)m_fpsCounter;

		m_fpsAccumulator = 0.0;
		m_timeSinceFPSUpdate = 0.0;
		m_fpsCounter = 0;

		std::string title =
			"FPS: " + std::to_string( (int)m_averageFps ) +
			" (" + std::to_string( 1000.0 / m_averageFps ) + "ms)   "
			"MAX: " + std::to_string( (int)m_maxFps );
		m_pEngine->context->setTitle( title.c_str() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////
