#include "Engine.h"

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <wv/Material/Material.h>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/GraphicsDevice.h>
#include <wv/Device/AudioDevice.h>

#include <wv/JobSystem/JobSystem.h>

#include <wv/Memory/FileSystem.h>
#include <wv/Memory/ModelParser.h>

#include <wv/Physics/PhysicsEngine.h>
#include <wv/Mesh/MeshResource.h>

#include <wv/Graphics/RenderTarget.h>
#include <wv/RenderTarget/IntermediateRenderTargetHandler.h>
#include <wv/Resource/ResourceRegistry.h>
#include <wv/Shader/ShaderResource.h>

#include <wv/Engine/EngineReflect.h>

#include <wv/Engine/ApplicationState.h>
#include <wv/Events/Dispatcher.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

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

	m_screenRenderTarget = graphics->m_renderTargets.emplace();
	sRenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );
	rt.width = _desc->windowWidth;
	rt.height = _desc->windowHeight;
	rt.fbHandle = 0;
	
	m_pApplicationState = _desc->pApplicationState;
	m_pApplicationState->initialize();

	/// TODO: move to descriptor
	m_pPhysicsEngine = WV_NEW( cJoltPhysicsEngine );
	m_pPhysicsEngine->init();
	
	const int concurrency = std::thread::hardware_concurrency();
	m_pJobSystem = WV_NEW( JobSystem );
	m_pJobSystem->initialize( wv::Math::max( 0,  - 1 ) );

	m_pFileSystem = _desc->systems.pFileSystem;
	m_pResourceRegistry = WV_NEW( cResourceRegistry, m_pFileSystem, graphics, m_pJobSystem );

	graphics->initEmbeds();

	audio = _desc->device.pAudio;

	/* 
	 * create deferred rendering objects
	 * this should be configurable
	 */

#ifndef WV_PLATFORM_PSVITA // use forward rendering on vita
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Deferred Resources\n" );
	{ 
		m_pDeferredShader = WV_NEW( cShaderResource, "deferred" );
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

wv::cEngine* wv::cEngine::get()
{
	return s_pInstance;
}

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

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

void wv::cEngine::handleInput()
{
	sMouseEvent mouseEvent;
	while ( m_mouseListener.pollEvent( mouseEvent ) )
	{
		m_mousePosition = mouseEvent.position;
	}
	

	sInputEvent inputEvent;
	while ( m_inputListener.pollEvent( inputEvent ) )
	{
		if ( inputEvent.buttondown )
		{
			switch ( inputEvent.key )
			{
			case 'R': m_pApplicationState->reloadScene(); break;
			case 'F': m_drawWireframe ^= 1; break;

			case WV_KEY_ESCAPE: context->close(); break;

			default: 
				break;
			}

		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

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
	return wv::Vector2i{
		context->getWidth(),
		context->getHeight()
	};
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::run()
{

	// Subscribe to user input event
	m_inputListener.hook();
	m_mouseListener.hook();

	orbitCamera = WV_NEW( OrbitCamera, iCamera::WV_CAMERA_TYPE_PERSPECTIVE );
	freeflightCamera = WV_NEW( FreeflightCamera, iCamera::WV_CAMERA_TYPE_PERSPECTIVE );
	orbitCamera->onEnter();
	freeflightCamera->onEnter();

	freeflightCamera->getTransform().setPosition( { 0.0f, 0.0f, 20.0f } );

	currentCamera = freeflightCamera;
	
	size_t embeddedResources = m_pResourceRegistry->getNumLoadedResources();

	m_pApplicationState->switchToScene( 0 ); // default scene
	
	// wait for load to be done
	m_pResourceRegistry->waitForFence();

#ifdef EMSCRIPTEN
	emscripten_set_main_loop( []{ wv::cEngine::get()->tick(); }, 0, 1);
#else

	int timeToDeath = 5;
	int deathCounter = 0;
	double deathTimer = 0.0;
	while( context->isAlive() )
	{
		tick();

		// automatic shutdown if the context is NONE
		if( context->getContextAPI() == WV_DEVICE_CONTEXT_API_NONE )
		{
			double t = context->getTime();
			
			deathTimer = t - static_cast<double>( deathCounter );
			if( deathTimer > 1.0 )
			{
				wv::Debug::Print( "Automatic Close in: %i\n", timeToDeath - deathCounter );
				deathCounter++;
			}

			if( deathCounter > timeToDeath )
				context->close();
		}
	}

#endif


	Debug::Print( Debug::WV_PRINT_DEBUG, "Quitting...\n" );
	
	m_pApplicationState->onExit();
	m_pApplicationState->onDestruct();
	
	// wait for unload to be done
	m_pResourceRegistry->waitForFence();

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::terminate()
{
	graphics->executeCreateQueue();

	currentCamera = nullptr;
	
	if( orbitCamera )
	{
		WV_FREE( orbitCamera );
		orbitCamera = nullptr;
	}

	if( freeflightCamera )
	{
		WV_FREE( freeflightCamera );
		freeflightCamera = nullptr;
	}

	graphics->destroyMesh( m_screenQuad );
	graphics->destroyRenderTarget( m_gbuffer );

	if( m_pDeferredShader )
	{
		m_pDeferredShader->unload( m_pFileSystem, graphics );
		WV_FREE( m_pDeferredShader );
		m_pDeferredShader = nullptr;
	}

	// destroy modules
	Debug::Draw::Internal::deinitDebugDraw( graphics );
	
	if( m_pApplicationState )
	{
		m_pApplicationState->terminate();
		WV_FREE( m_pApplicationState );
		m_pApplicationState = nullptr;
	}

	if ( m_pPhysicsEngine )
	{
		m_pPhysicsEngine->terminate();
		WV_FREE( m_pPhysicsEngine );
		m_pPhysicsEngine = nullptr;
	}

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
	
	if( graphics )
	{
		graphics->terminate();
		WV_FREE( graphics );
		graphics = nullptr;
	}

	if( m_pResourceRegistry )
	{
		WV_FREE( m_pResourceRegistry );
		m_pResourceRegistry = nullptr;
	}

	if ( m_pJobSystem )
	{
		m_pJobSystem->terminate();
		WV_FREE( m_pJobSystem );
		m_pJobSystem = nullptr;
	}

	if( m_pFileSystem )
	{
		WV_FREE( m_pFileSystem );
		m_pFileSystem = nullptr;
	}

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
			cEngine::get()->m_pPhysicsEngine->update( *(double*)_pUserData );
		};
	Job* job = m_pJobSystem->createJob( physicsFence, nullptr, fptr, &dt );
	m_pJobSystem->submit( { job } );
#endif

	m_pApplicationState->onUpdate( dt );
	currentCamera->update( dt );
	
#ifdef WV_SUPPORT_JOLT_PHYSICS
	m_pJobSystem->waitAndDeleteFence( physicsFence );
#endif


	/// ------------------ render ------------------ ///	

#ifndef WV_PLATFORM_PSVITA
	if( !m_gbuffer.is_valid() )
		return;
#endif

	{
		sRenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );
		if ( rt.width == 0 || rt.height == 0 )
			return;
	}

#ifdef WV_PLATFORM_PSVITA
	graphics->cmdBeginRender( {}, {} );
#else
	graphics->cmdBeginRender( {}, m_gbuffer );
#endif

	graphics->beginRender();
	
#ifdef WV_SUPPORT_IMGUI
	if( context->newImGuiFrame() )
	{
		ImGui::DockSpaceOverViewport( 0, 0, ImGuiDockNodeFlags_PassthruCentralNode );
	}
#endif // WV_SUPPORT_IMGUI
	
	if ( currentCamera->beginRender( graphics, m_drawWireframe ? WV_FILL_MODE_WIREFRAME : WV_FILL_MODE_SOLID ) )
	{
		graphics->cmdClearColor( {}, 0.0f, 0.0f, 0.0f, 1.0f );
		graphics->cmdClearDepthStencil( {}, 1.0, 0 );

		m_pApplicationState->onDraw( context, graphics );
		m_pResourceRegistry->drawMeshInstances();

	#ifdef WV_DEBUG
		Debug::Draw::Internal::drawDebug( graphics );
	#endif
	}

#ifndef WV_PLATFORM_PSVITA
	graphics->cmdBeginRender( {}, m_screenRenderTarget );
	
	graphics->cmdClearColor( {}, 0.0f, 0.0f, 0.0f, 1.0f );
	graphics->cmdClearDepthStencil( {}, 1.0, 0 );

	// bind gbuffer textures to deferred pass
	{
		sRenderTarget& rt = graphics->m_renderTargets.at( m_gbuffer );
		for ( int i = 0; i < rt.numTextures; i++ )
			graphics->bindTextureToSlot( rt.pTextureIDs[ i ], i );
	}

	// render screen quad with deferred shader
	if ( m_pDeferredShader->isComplete() && currentCamera->beginRender( graphics, WV_FILL_MODE_SOLID ) )
	{
		m_pDeferredShader->bind( graphics );

		GPUBufferID UbFogParams  = m_pDeferredShader->getShaderBuffer( "UbFogParams" );
		GPUBufferID SbVerticesID = m_pDeferredShader->getShaderBuffer( "SbVertices" );
		sGPUBuffer fogParamBuffer = graphics->m_gpuBuffers.at( UbFogParams );

		graphics->bufferSubData( UbFogParams, &m_fogParams, sizeof( m_fogParams ), 0 );
		graphics->bindBufferIndex( UbFogParams, fogParamBuffer.bindingIndex.value );

		sMesh screenQuad = graphics->m_meshes.at( m_screenQuad );
		{
			wv::sGPUBuffer& SbVertices = graphics->m_gpuBuffers.at( SbVerticesID );
			graphics->bindBufferIndex( screenQuad.vertexBufferID, SbVertices.bindingIndex.value );
			graphics->cmdBindIndexBuffer( {}, screenQuad.indexBufferID, 0, {} );
		}

		// sGPUBuffer& ibuffer = graphics->m_gpuBuffers.at( screenQuad.indexBufferID );
		graphics->cmdDrawIndexed( {}, screenQuad.numIndices, 1, 0, 0, 0 );
	}
#endif

#ifdef WV_SUPPORT_IMGUI
	context->renderImGui();
#endif

	graphics->endRender();

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
		prDesc.vertices     = (uint8_t*)vertices.data();
		prDesc.sizeVertices = vertices.size() * sizeof( Vertex );

		prDesc.pIndices32 = indices.data();
		prDesc.numIndices = indices.size();

		prDesc.deleteData = false;
	}

	m_screenQuad = graphics->createMesh( prDesc );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::createGBuffer()
{
	Vector2i size = getViewportSize();

	sRenderTargetDesc rtDesc;
	rtDesc.width  = size.x;
	rtDesc.height = size.y;
#ifdef WV_PLATFORM_WINDOWS
	sTextureDesc texDescs[] = {
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_BYTE }, // color
		{ wv::WV_TEXTURE_CHANNELS_RG,   wv::WV_TEXTURE_FORMAT_BYTE }, // normal
		{ wv::WV_TEXTURE_CHANNELS_RG,   wv::WV_TEXTURE_FORMAT_FLOAT } // roughness/metallic
	};
	rtDesc.pTextureDescs = texDescs;
	rtDesc.numTextures = 3;
#endif

	m_gbuffer = graphics->createRenderTarget( rtDesc );
	graphics->executeCreateQueue();


}

void wv::cEngine::recreateScreenRenderTarget( int _width, int _height )
{
	{
		sRenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );

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

void wv::cEngine::_physicsUpdate( double _deltaTime )
{
	m_pApplicationState->onPhysicsUpdate( _deltaTime );
}

