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
#include <wv/renderer/mesh_renderer.h>

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

#ifndef WV_PACKAGE
	m_pThreadProfiler = WV_NEW( ThreadProfiler );
#endif 

	const int concurrency = std::thread::hardware_concurrency();
	m_pJobSystem = WV_NEW( JobSystem );
	//m_pJobSystem->initialize( wv::Math::max( 0, concurrency - 1 ) );
	m_pJobSystem->initialize( 0 );

	m_pFileSystem = _desc->systems.pFileSystem;
	m_pFileSystem->initialize();

	m_pResourceRegistry = WV_NEW( ResourceRegistry, m_pFileSystem, graphics, m_pJobSystem );

	graphics->initEmbeds();

	audio = _desc->device.pAudio;

	m_pMeshRenderer = WV_NEW( IMeshRenderer, graphics );

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

void wv::Engine::terminate()
{
	graphics->destroyMesh( m_screenQuad );
	graphics->destroyRenderTarget( m_gbuffer );
	graphics->deinitEmbeds();

	if ( m_pMeshRenderer )
	{
		m_pMeshRenderer->flush();
		WV_FREE( m_pMeshRenderer );
	}

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

#ifndef WV_PACKAGE
	if( m_pThreadProfiler )
	{
		WV_FREE( m_pThreadProfiler );
		m_pThreadProfiler = nullptr;
	}
#endif

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

void wv::Engine::quit()
{
	context->close();
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
