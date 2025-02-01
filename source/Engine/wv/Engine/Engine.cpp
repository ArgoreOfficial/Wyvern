#include "Engine.h"

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <wv/Material/Material.h>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/Graphics.h>
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


	m_screenRenderTarget = graphics->m_renderTargets.emplace();
	sRenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );
	rt.width = _desc->windowWidth;
	rt.height = _desc->windowHeight;
	rt.fbHandle = 0;
	
	m_pApplicationState = _desc->pApplicationState;

	/// TODO: move to descriptor
	m_pPhysicsEngine = new cJoltPhysicsEngine();
	m_pPhysicsEngine->init();
	

	m_pFileSystem = _desc->systems.pFileSystem;
	m_pResourceRegistry = new cResourceRegistry( m_pFileSystem, graphics );
	m_pResourceRegistry->initializeEmbeded();

	m_pJobSystem = new JobSystem();
	m_pJobSystem->initialize( 3 );

	graphics->initEmbeds();

	/* 
	 * create deferred rendering objects
	 * this should be configurable
	 */

#ifndef WV_PLATFORM_PSVITA // use forward rendering on vita
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Deferred Resources\n" );
	{ 
		m_pDeferredShader = new cShaderResource( "deferred" );
		m_pDeferredShader->load( m_pFileSystem, graphics );
		
		createScreenQuad();
		createGBuffer();
	}
#endif
	
	graphics->setRenderTarget( m_screenRenderTarget );
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
			}

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
		sRenderTarget& rt = graphics->m_renderTargets.at( m_pIRTHandler->m_renderTarget );

		if( m_pIRTHandler->m_renderTarget.is_valid() )
			return { rt.width, rt.height };
		else
			return { 1,1 };
	}
	
	return { context->getWidth(), context->getHeight() };
}


void wv::cEngine::run()
{

	// Subscribe to user input event
	m_inputListener.hook();
	m_mouseListener.hook();

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
	
	m_pJobSystem->terminate();
	delete m_pJobSystem;
	m_pJobSystem = nullptr;

	if ( m_pPhysicsEngine )
	{
		m_pPhysicsEngine->terminate();
		delete m_pPhysicsEngine;
		m_pPhysicsEngine = nullptr;
	}

	delete orbitCamera;
	delete freeflightCamera;
	orbitCamera = nullptr;
	freeflightCamera = nullptr;

	graphics->destroyMesh( m_screenQuad );
	graphics->destroyRenderTarget( m_gbuffer );

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
	if( !m_gbuffer.is_valid() )
		return;
#endif

	{
		sRenderTarget& rt = graphics->m_renderTargets.at( m_screenRenderTarget );
		if ( rt.width == 0 || rt.height == 0 )
			return;
	}

#ifdef WV_PLATFORM_PSVITA
	graphics->setRenderTarget( nullptr );
#else
	graphics->setRenderTarget( m_gbuffer );
#endif

	graphics->beginRender();
	
#ifdef WV_SUPPORT_IMGUI
	context->newImGuiFrame();
	ImGui::DockSpaceOverViewport( 0, 0, ImGuiDockNodeFlags_PassthruCentralNode );
#endif // WV_SUPPORT_IMGUI
	
	if ( currentCamera->beginRender( graphics, m_drawWireframe ? WV_FILL_MODE_WIREFRAME : WV_FILL_MODE_SOLID ) )
	{
		graphics->clearRenderTarget( true, true );

		m_pApplicationState->draw( context, graphics );
		m_pResourceRegistry->drawMeshInstances();

		GPUBufferID viewBufferID = currentCamera->getBufferID();

		for ( auto& pair : graphics->m_pipelineDrawListMap )
		{
			PipelineID pipelineID = pair.first;
			DrawListID drawListID = pair.second;
			sDrawList& drawList = graphics->m_drawLists.at( drawListID );

			if ( drawList.cmds.empty() || !drawList.instanceBufferID.is_valid() )
				continue;

			graphics->bindPipeline( pipelineID );
			graphics->bindVertexBuffer( drawList.vertexBufferID );

			sGPUBuffer instanceBuffer = graphics->m_gpuBuffers.at( drawList.instanceBufferID );
			graphics->bufferData( drawList.instanceBufferID, drawList.instances.data(), drawList.instances.size() * sizeof( sMeshInstanceData ) );
			graphics->bindBufferIndex( drawList.instanceBufferID, instanceBuffer.bindingIndex.value );

			sGPUBuffer viewBuffer = graphics->m_gpuBuffers.at( drawList.viewDataBufferID );
			graphics->bindBufferIndex( viewBufferID, viewBuffer.bindingIndex.value );
			
			graphics->multiDrawIndirect( drawListID );
			drawList.instances.clear();
			drawList.cmds.clear();
		}


	#ifdef WV_DEBUG
		Debug::Draw::Internal::drawDebug( graphics );
	#endif
	}

#ifndef WV_PLATFORM_PSVITA
	if( m_pIRTHandler )
	{
		if( m_pIRTHandler->m_renderTarget.is_valid() )
			graphics->setRenderTarget( m_pIRTHandler->m_renderTarget );
	}
	else
		graphics->setRenderTarget( m_screenRenderTarget );
	
	graphics->clearRenderTarget( true, true );

	// bind gbuffer textures to deferred pass
	{
		sRenderTarget& rt = graphics->m_renderTargets.at( m_gbuffer );
		for ( int i = 0; i < rt.numTextures; i++ )
			graphics->bindTextureToSlot( rt.pTextureIDs[ i ], i );
	}

	// render screen quad with deferred shader
	if ( currentCamera->beginRender( graphics, WV_FILL_MODE_SOLID ) )
	{
		m_pDeferredShader->bind( graphics );

		GPUBufferID UbFogParams  = m_pDeferredShader->getShaderBuffer( "UbFogParams" );
		GPUBufferID SbVerticesID = m_pDeferredShader->getShaderBuffer( "SbVertices" );
		sGPUBuffer fogParamBuffer = graphics->m_gpuBuffers.at( UbFogParams );

		graphics->bufferSubData( UbFogParams, &m_fogParams, sizeof( m_fogParams ), 0 );
		graphics->bindBufferIndex( UbFogParams, fogParamBuffer.bindingIndex.value );

		sMesh screenQuad = graphics->m_meshes.at( m_screenQuad );
		graphics->bindVertexBuffer( SbVerticesID );
		
		// sGPUBuffer& ibuffer = graphics->m_gpuBuffers.at( screenQuad.indexBufferID );
		graphics->drawIndexed( screenQuad.numIndices );
	}

	if( m_pIRTHandler )
	{
		if ( currentCamera->beginRender( graphics, WV_FILL_MODE_SOLID ) )
		{
			graphics->setRenderTarget( m_screenRenderTarget );
			graphics->clearRenderTarget( true, true );
			m_pIRTHandler->draw( graphics );
		}
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

			if( m_pIRTHandler->m_renderTarget.is_valid() )
			{
				sRenderTarget& rt = graphics->m_renderTargets.at( m_pIRTHandler->m_renderTarget );
				recreateScreenRenderTarget( rt.width, rt.height );
			}
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

	m_screenQuad = graphics->createMesh( {}, &prDesc );
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

	CmdBufferID buffer = graphics->getCommandBuffer();
	m_gbuffer = graphics->cmdCreateRenderTarget( buffer, rtDesc );
	graphics->submitCommandBuffer( buffer );
	graphics->executeCommandBuffer( buffer );


}

void wv::cEngine::recreateScreenRenderTarget( int _width, int _height )
{
	graphics->onResize( _width, _height );
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

