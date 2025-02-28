#include "Engine.h"

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <wv/material/material.h>

#include <wv/camera/freeflight_camera.h>
#include <wv/camera/orbit_camera.h>

#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>
#include <wv/device/audio_device.h>
#include <wv/debug/thread_profiler.h>

#include <wv/job/job_system.h>

#include <wv/filesystem/file_system.h>
#include <wv/memory/model_parser.h>

#include <wv/physics/physics_engine.h>
#include <wv/mesh/mesh_resource.h>

#include <wv/graphics/render_target.h>
#include <wv/Resource/resource_registry.h>
#include <wv/shader/shader_resource.h>

#include <wv/engine_reflect.h>

#include <wv/app_state.h>
#include <wv/event/dispatcher.h>

#include <wv/debug/log.h>
#include <wv/debug/Draw.h>

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>

#include <type_traits>
#include <random>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif // EMSCRIPTEN

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif // WV_SUPPORT_IMGUI

///////////////////////////////////////////////////////////////////////////////////////

wv::Engine* wv::Engine::s_pInstance = nullptr;

///////////////////////////////////////////////////////////////////////////////////////

wv::Engine::Engine( EngineDesc* _desc )
{
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Engine\n" );

	if ( !_desc->pAppState )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "No application state provided. Cannot create application\n" );
		return;
	}

	s_pInstance = this;

	context  = _desc->device.pContext;
	graphics = _desc->device.pGraphics;

	m_screenRenderTarget = graphics->m_renderTargets.emplace();
	RenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );
	rt.width = _desc->windowWidth;
	rt.height = _desc->windowHeight;
	rt.fbHandle = 0;
	
	m_pAppState = _desc->pAppState;
	m_pAppState->initialize();

#ifdef WV_SUPPORT_PHYSICS
	/// TODO: move to descriptor
	m_pPhysicsEngine = WV_NEW( JoltPhysicsEngine );
	m_pPhysicsEngine->init();
#endif

	m_pThreadProfiler = WV_NEW( ThreadProfiler );

	const int concurrency = std::thread::hardware_concurrency();
	m_pJobSystem = WV_NEW( JobSystem );
	m_pJobSystem->initialize( wv::Math::max( 0, concurrency - 1 ) );

	m_pFileSystem = _desc->systems.pFileSystem;
	m_pFileSystem->initialize();

	m_pResourceRegistry = WV_NEW( ResourceRegistry, m_pFileSystem, graphics, m_pJobSystem );

	graphics->initEmbeds();

	audio = _desc->device.pAudio;

	/* 
	 * create deferred rendering objects
	 * this should be configurable
	 */

#ifndef WV_PLATFORM_PSVITA // use forward rendering on vita
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Deferred Resources\n" );
	{ 
		m_pDeferredShader = WV_NEW( ShaderResource, "deferred" );
		m_pDeferredShader->load( m_pFileSystem, graphics );
		
		createScreenQuad();
		createGBuffer();
	}
#endif
	
	initImgui();

	Debug::Print( Debug::WV_PRINT_WARN, "TODO: Create AudioDeviceDesc\n" );
	Debug::Print( Debug::WV_PRINT_DEBUG, "Initializing Debug Draw\n" );
	Debug::Draw::Internal::initDebugDraw( graphics, m_pResourceRegistry );

	Debug::Print( "Created Engine\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Engine* wv::Engine::get()
{
	return s_pInstance;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::UUID wv::Engine::getUniqueUUID()
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

///////////////////////////////////////////////////////////////////////////////////////

wv::Handle wv::Engine::getUniqueHandle()
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

void wv::Engine::onResize( int _width, int _height )
{
	context->onResize( _width, _height );
	recreateScreenRenderTarget( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::handleInput()
{
	MouseEvent mouseEvent;
	while ( m_mouseListener.pollEvent( mouseEvent ) )
	{
		m_mousePosition = mouseEvent.position;
	}
	

	InputEvent inputEvent;
	while ( m_inputListener.pollEvent( inputEvent ) )
	{
		if ( inputEvent.buttondown )
		{
			switch ( inputEvent.key )
			{
			case 'R': m_pAppState->reloadScene(); break;
			case 'F': m_drawWireframe ^= 1; break;

			case WV_KEY_ESCAPE: context->close(); break;

			default: 
				break;
			}

		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::setSize( int _width, int _height, bool _notify )
{
	context->setSize( _width, _height );
	
	if( _notify )
		onResize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

#ifdef EMSCRIPTEN
void emscriptenMainLoop() { wv::Engine::get()->tick(); }
#endif

///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::run()
{
	// Subscribe to user input event
	m_inputListener.hook();
	m_mouseListener.hook();

	size_t embeddedResources = m_pResourceRegistry->getNumLoadedResources();

	m_pAppState->switchToScene( 0 ); // default scene
	
	// wait for load to be done
	m_pResourceRegistry->waitForFence();

#ifdef EMSCRIPTEN
	emscripten_set_main_loop( []{ wv::Engine::get()->tick(); }, 0, 1);
#else

	int timeToDeath = 5;
	int deathCounter = 0;
	double deathTimer = 0.0;
	while( context->isAlive() )
	{
	#ifndef WV_PACKAGE
		m_pThreadProfiler->begin();
	#endif
		
		tick();

	#ifndef WV_PACKAGE
		m_pThreadProfiler->end();
	#endif

		// automatic shutdown if the context is NONE
		if( context->getContextAPI() == WV_DEVICE_CONTEXT_API_NONE )
		{
			double t = context->getTime();
			
			deathTimer = t - static_cast<double>( deathCounter );
			if( deathTimer > 1.0 )
			{
				wv::Debug::Print( "AWESOME SAUCE AND COOL: %i\n", timeToDeath - deathCounter );
				deathCounter++;
			}

			if( deathCounter > timeToDeath )
				context->close();
		}
	}

#endif


	Debug::Print( Debug::WV_PRINT_DEBUG, "Quitting...\n" );
	
	m_pAppState->onExit();
	m_pAppState->onDestruct();
	
	// wait for unload to be done
	m_pResourceRegistry->waitForFence();

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::terminate()
{
	graphics->destroyMesh( m_screenQuad );
	graphics->destroyRenderTarget( m_gbuffer );
	graphics->deinitEmbeds();

	if( m_pDeferredShader )
	{
		m_pDeferredShader->unload( m_pFileSystem, graphics );
		WV_FREE( m_pDeferredShader );
		m_pDeferredShader = nullptr;
	}

	// destroy modules
	Debug::Draw::Internal::deinitDebugDraw( graphics );
	
	if( m_pAppState )
	{
		m_pAppState->terminate();
		WV_FREE( m_pAppState );
		m_pAppState = nullptr;
	}

#ifdef WV_SUPPORT_PHYSICS
	if ( m_pPhysicsEngine )
	{
		m_pPhysicsEngine->terminate();
		WV_FREE( m_pPhysicsEngine );
		m_pPhysicsEngine = nullptr;
	}
#endif

	if( audio )
	{
		audio->terminate();
		WV_FREE( audio );
		audio = nullptr;
	}

	if( context )
	{
		context->terminate();
		WV_FREE( context );
		context = nullptr;
	}
	
	if( m_pResourceRegistry )
	{
		m_pResourceRegistry->waitForFence();
		WV_FREE( m_pResourceRegistry );
		m_pResourceRegistry = nullptr;
	}

	if ( m_pJobSystem )
	{
		m_pJobSystem->terminate();
		WV_FREE( m_pJobSystem );
		m_pJobSystem = nullptr;
	}

	if( m_pThreadProfiler )
	{
		WV_FREE( m_pThreadProfiler );
		m_pThreadProfiler = nullptr;
	}

	if( m_pFileSystem )
	{
		WV_FREE( m_pFileSystem );
		m_pFileSystem = nullptr;
	}

	if( graphics )
	{
		graphics->terminate();
		WV_FREE( graphics );
		graphics = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::tick()
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
	handleInput();

#ifdef WV_PLATFORM_WINDOWS
	// refresh fps display only on windows
	{
		double fps = 1.0 / dt;
		if ( fps > m_maxFps )
			m_maxFps = fps;

		m_timeSinceFPSUpdate += dt;
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
				" ("     + std::to_string( 1000.0 / m_averageFps ) + "ms)   "
				"MAX: " + std::to_string( (int)m_maxFps );
			context->setTitle( title.c_str() );
		}
	}
#endif

#ifdef WV_SUPPORT_JOLT_PHYSICS
	Fence* physicsFence = m_pJobSystem->createFence();
	Job::JobFunction_t fptr = []( void* _pUserData )
		{
			Engine::get()->m_pPhysicsEngine->update( *(double*)_pUserData );
		};
	Job* job = m_pJobSystem->createJob( physicsFence, nullptr, fptr, &dt );
	m_pJobSystem->submit( { job } );
#endif

	m_pAppState->onUpdate( dt );
	m_pAppState->currentCamera->update( dt );
	
#ifdef WV_SUPPORT_JOLT_PHYSICS
	m_pJobSystem->waitAndDeleteFence( physicsFence );
#endif


	/// ------------------ render ------------------ ///	

#ifdef WV_PLATFORM_WINDOWS
	if( !m_gbuffer.is_valid() )
		return;
#endif

	{
		RenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );
		if ( rt.width == 0 || rt.height == 0 )
			return;
	}

#ifdef WV_PLATFORM_WINDOWS
	graphics->cmdBeginRender( {}, m_gbuffer );
#else
	graphics->cmdBeginRender( {}, {} );
#endif

	graphics->beginRender();
	
#ifdef WV_SUPPORT_IMGUI
	if( context->newImGuiFrame() )
	{
		ImGui::DockSpaceOverViewport( 0, 0, ImGuiDockNodeFlags_PassthruCentralNode );
	}
#endif // WV_SUPPORT_IMGUI
	
	if ( m_pAppState->currentCamera->beginRender( graphics, m_drawWireframe ? WV_FILL_MODE_WIREFRAME : WV_FILL_MODE_SOLID ) )
	{
		graphics->cmdClearColor( {}, 0.0f, 0.0f, 0.0f, 1.0f );
		graphics->cmdClearDepthStencil( {}, 1.0, 0 );

		m_pAppState->onDraw( context, graphics );
		m_pResourceRegistry->drawMeshInstances();

	#ifdef WV_DEBUG
		Debug::Draw::Internal::drawDebug( graphics );
	#endif
	}

#ifdef WV_PLATFORM_WINDOWS
	graphics->cmdBeginRender( {}, m_screenRenderTarget );
	
	graphics->cmdClearColor( {}, 0.0f, 0.0f, 0.0f, 1.0f );
	graphics->cmdClearDepthStencil( {}, 1.0, 0 );

	// bind gbuffer textures to deferred pass
	{
		RenderTarget& rt = graphics->m_renderTargets.at( m_gbuffer );
		for ( int i = 0; i < rt.numTextures; i++ )
			graphics->bindTextureToSlot( rt.pTextureIDs[ i ], i );
	}

	// render screen quad with deferred shader
	if ( m_pDeferredShader->isComplete() && m_pAppState->currentCamera->beginRender( graphics, WV_FILL_MODE_SOLID ) )
	{
		m_pDeferredShader->bind( graphics );

		GPUBufferID UbFogParams  = m_pDeferredShader->getShaderBuffer( "UbFogParams" );
		GPUBufferID SbVerticesID = m_pDeferredShader->getShaderBuffer( "SbVertices" );
		GPUBuffer fogParamBuffer = graphics->m_gpuBuffers.at( UbFogParams );

		graphics->bufferSubData( UbFogParams, &m_fogParams, sizeof( m_fogParams ), 0 );
		graphics->bindBufferIndex( UbFogParams, fogParamBuffer.bindingIndex.value );

		Mesh screenQuad = graphics->m_meshes.at( m_screenQuad );
		{
			wv::GPUBuffer& SbVertices = graphics->m_gpuBuffers.at( SbVerticesID );
			graphics->bindBufferIndex( screenQuad.vertexBufferID, SbVertices.bindingIndex.value );
			graphics->cmdBindIndexBuffer( {}, screenQuad.indexBufferID, 0, {} );
		}

		// GPUBuffer& ibuffer = graphics->m_gpuBuffers.at( screenQuad.indexBufferID );
		graphics->cmdDrawIndexed( {}, screenQuad.numIndices, 1, 0, 0, 0 );
	}
#endif

#ifdef WV_SUPPORT_IMGUI
	context->renderImGui();
#endif

	graphics->endRender();

	context->swapBuffers();
}

void wv::Engine::quit()
{
	shutdownImgui();
	context->close();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::initImgui()
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

void wv::Engine::shutdownImgui()
{
#ifdef WV_SUPPORT_IMGUI
	context->terminateImGui();
	ImGui::DestroyContext();
#endif // WV_SUPPORT_IMGUI
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::createScreenQuad()
{
	std::vector<Vertex> vertices;
	vertices.push_back( Vertex{ {  3.0f, -1.0f, 0.5f }, {}, {}, {}, { 2.0f, 0.0f } } );
	vertices.push_back( Vertex{ { -1.0f,  3.0f, 0.5f }, {}, {}, {}, { 0.0f, 2.0f } } );
	vertices.push_back( Vertex{ { -1.0f, -1.0f, 0.5f }, {}, {}, {}, { 0.0f, 0.0f } } );
	
	std::vector<uint32_t> indices;
	indices.push_back( 0 );
	indices.push_back( 1 );
	indices.push_back( 2 );
	wv::MeshDesc prDesc;
	{
		prDesc.vertices     = (uint8_t*)vertices.data();
		prDesc.sizeVertices = vertices.size() * sizeof( Vertex );

		prDesc.pIndices32 = indices.data();
		prDesc.numIndices = indices.size();

		prDesc.deleteData = false;
	}

	m_screenQuad = graphics->createMesh( prDesc );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Engine::createGBuffer()
{
	Vector2i size = context->getSize();

	RenderTargetDesc rtDesc;
	rtDesc.width  = size.x;
	rtDesc.height = size.y;
#ifdef WV_PLATFORM_WINDOWS
	TextureDesc texDescs[] = {
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_BYTE }, // color
		{ wv::WV_TEXTURE_CHANNELS_RG,   wv::WV_TEXTURE_FORMAT_BYTE }, // normal
		{ wv::WV_TEXTURE_CHANNELS_RG,   wv::WV_TEXTURE_FORMAT_FLOAT } // roughness/metallic
	};
	rtDesc.pTextureDescs = texDescs;
	rtDesc.numTextures = 3;
#endif

	m_gbuffer = graphics->createRenderTarget( rtDesc );
}

void wv::Engine::recreateScreenRenderTarget( int _width, int _height )
{
	{
		RenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );

		// recreate render target
		rt.width = _width;
		rt.height = _height;
	}
	
	if( m_gbuffer.is_valid() )
		graphics->destroyRenderTarget( m_gbuffer );
	
	if( _width == 0 || _height == 0 )
		return;

	createGBuffer();
}
