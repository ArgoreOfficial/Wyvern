#include "Engine.h"

#include <glad/glad.h>

#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Texture.h>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/AudioDevice.h>

#include <wv/Memory/MemoryDevice.h>
#include <wv/Memory/ModelParser.h>

#include <wv/Primitive/Mesh.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <wv/Scene/Model.h>
#include <wv/Shader/ShaderRegistry.h>
#include <wv/State/State.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <SDL2/SDL_keycode.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


///////////////////////////////////////////////////////////////////////////////////////

wv::cEngine::cEngine( EngineDesc* _desc )
{
	if ( !_desc->applicationState )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "! NO APPLICATION STATE WAS PROVIDED !\n" );
		return;
	}

	s_instance = this;

	context  = _desc->device.pContext;
	graphics = _desc->device.pGraphics;

	m_pFileSystem     = _desc->systems.pFileSystem;
	m_pShaderRegistry = _desc->systems.pShaderRegistry;

	m_defaultRenderTarget = new RenderTarget();
	m_defaultRenderTarget->width  = _desc->windowWidth;
	m_defaultRenderTarget->height = _desc->windowHeight;
	m_defaultRenderTarget->fbHandle = 0;

	m_applicationState = _desc->applicationState;

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

	Debug::Print( Debug::WV_PRINT_WARN, "TODO: Create AudioDeviceDesc\n" );
	

	Debug::Draw::Internal::initDebugDraw( graphics );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cEngine* wv::cEngine::get()
{
	return s_instance;
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

	currentCamera = orbitCamera;

	if ( m_applicationState )
	{
		m_applicationState->onCreate();
		m_applicationState->onLoad(); /// TODO: multithread
		// while m_applicationState->isLoading() { doloadingstuff }
	}

#ifdef EMSCRIPTEN
	emscripten_set_main_loop( []{ wv::cEngine::get()->tick(); }, 0, 1);
#else
	while ( context->isAlive() )
		tick();
#endif

	Debug::Print( Debug::WV_PRINT_DEBUG, "Quitting...\n" );

	if ( m_applicationState )
	{
		m_applicationState->onUnload();
		m_applicationState->onDestroy();
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::terminate()
{
	currentCamera = nullptr;
	
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


	// update modules
	m_pShaderRegistry->update();

	if( m_applicationState )
		m_applicationState->update( dt );

	currentCamera->update( dt );

	/// ------------------ render ------------------ ///
	
	
	if ( !m_gbuffer || m_defaultRenderTarget->width == 0 || m_defaultRenderTarget->height == 0 )
		return;

	graphics->setRenderTarget( m_gbuffer );
	graphics->clearRenderTarget( true, true );

	if( m_applicationState )
		m_applicationState->draw( graphics );

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

	context->swapBuffers();
}

void wv::cEngine::quit()
{
	context->close();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::createScreenQuad()
{
	wv::InputLayoutElement elements[] = {
		{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 },
		{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }
	};

	wv::InputLayout layout;
	layout.elements = elements;
	layout.numElements = 2;

	float vertices[] = { 
		 3.0f, -1.0f, 0.5f,       2.0f, 0.0f,
		-1.0f,  3.0f, 0.5f,       0.0f, 2.0f,
		-1.0f, -1.0f, 0.5f,       0.0f, 0.0f,
	};
	unsigned int indices[] = {
		0, 1, 2
	};

	wv::PrimitiveDesc prDesc;
	{
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;

		prDesc.vertexBuffer     = vertices;
		prDesc.vertexBufferSize = sizeof( vertices );
		prDesc.numVertices      = 3;

		prDesc.indexBuffer     = indices;
		prDesc.indexBufferSize = sizeof( indices );
		prDesc.numIndices      = 3;
	}

	MeshDesc meshDesc;
	m_screenQuad = graphics->createMesh( &meshDesc );
	graphics->createPrimitive( &prDesc, m_screenQuad );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cEngine::createGBuffer()
{
	RenderTargetDesc rtDesc;
	//rtDesc.width  = context->getWidth();
	//rtDesc.height = context->getHeight();
	
	rtDesc.width  = 640;
	rtDesc.height = 480;

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
