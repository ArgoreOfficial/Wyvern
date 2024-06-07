#include "Application.h"

#include <glad/glad.h>

#include <wv/Assets/Texture.h>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>
#include <wv/Device/Context.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Memory/ModelParser.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Mesh.h>
#include <wv/RenderTarget/RenderTarget.h>
#include <wv/Scene/Model.h>

#include <wv/Assets/Materials/Material.h>

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

wv::Application::Application( ApplicationDesc* _desc )
{
#ifdef EMSCRIPTEN /// WebGL only supports OpenGL ES 2.0/3.0
	wv::ContextDesc ctxDesc = Context::contextPreset_OpenGLES2(); 
#else
	wv::ContextDesc ctxDesc = Context::contextPreset_OpenGL();
#endif
	ctxDesc.name = _desc->title;
	ctxDesc.width = _desc->windowWidth;
	ctxDesc.height = _desc->windowHeight;

	context = new wv::Context( &ctxDesc );

	wv::GraphicsDeviceDesc deviceDesc;
	deviceDesc.loadProc = context->getLoadProc();
	deviceDesc.graphicsApi = ctxDesc.graphicsApi; // must be same as context
	deviceDesc.graphicsApiVersion = ctxDesc.graphicsApiVersion; // must be same as context

	device = wv::GraphicsDevice::createGraphicsDevice( &deviceDesc );

	m_defaultRenderTarget = new RenderTarget();
	m_defaultRenderTarget->width = _desc->windowWidth;
	m_defaultRenderTarget->height = _desc->windowHeight;
	m_defaultRenderTarget->fbHandle = 0;
	
	s_instance = this;

	/* 
	 * create deferred rendering objects
	 * this should be configurable
	 */
	{ 
		m_deferredPipeline = Pipeline::loadFromFile( "res/shaders/deferred.wshader" );
		createScreeQuad();
		createGBuffer();
	}
	
	device->setRenderTarget( m_defaultRenderTarget );
	
	/// TODO: move to prefab or model file
	{
		wv::assimp::Parser parser;
		m_monke = parser.load( "res/meshes/monke.glb" );
		m_skybox = parser.load( "res/meshes/skysphere.glb" );

		Material* skyMaterial = new Material(); // memory leak
		skyMaterial->load( "res/materials/defaultSky.wmat" );
		m_skybox->primitives[ 0 ]->material = skyMaterial;

		Material* phongMaterial = new Material(); // memory leak
		phongMaterial->load( "res/materials/phong.wmat" );
		m_monke->primitives[ 0 ]->material = phongMaterial;
	}

	device->setClearColor( wv::Colors::Black );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Application* wv::Application::get()
{
	return s_instance;
}


///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::onResize( int _width, int _height )
{
	context->onResize( _width, _height );
	device->onResize( _width, _height );

	// recreate render target
	m_defaultRenderTarget->width = _width;
	m_defaultRenderTarget->height = _height;
	device->setRenderTarget( m_defaultRenderTarget );

	device->destroyRenderTarget( &m_gbuffer );
	
	if ( _width == 0 || _height == 0 )
		return;
	
	createGBuffer();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::onMouseEvent( MouseEvent _event )
{
	if ( _event.button != MouseEvent::WV_MOUSE_BUTTON_RIGHT )
		return;

	if ( !_event.buttondown )
		return;

	if ( currentCamera == orbitCamera )
	{
		currentCamera = freeflightCamera;
		freeflightCamera->getTransform().position = orbitCamera->getTransform().position;
		freeflightCamera->getTransform().rotation = orbitCamera->getTransform().rotation;

		( (FreeflightCamera*)freeflightCamera )->resetVelocity();
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::onInputEvent( InputEvent _event )
{
	if ( !_event.repeat )
		if ( _event.key == GLFW_KEY_F )
			if ( currentCamera != orbitCamera )
				currentCamera = orbitCamera; // lol
}

///////////////////////////////////////////////////////////////////////////////////////

#ifdef EMSCRIPTEN
void emscriptenMainLoop() { wv::Application::get()->tick(); }
#endif

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::run()
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

#ifdef EMSCRIPTEN
	emscripten_set_main_loop( []{ wv::Application::get()->tick(); }, 0, 1);
#else
	while ( context->isAlive() )
		tick();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::terminate()
{
	currentCamera = nullptr;
	
	delete orbitCamera;
	delete freeflightCamera;
	orbitCamera = nullptr;
	freeflightCamera = nullptr;

	device->destroyMesh( &m_screenQuad );
	device->destroyPipeline( &m_deferredPipeline );
	device->destroyRenderTarget( &m_gbuffer );

	context->terminate();
	device->terminate();
	
	delete context;
	delete device;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::tick()
{
	double dt = context->getDeltaTime();
	
	/// ------------------ update ------------------ ///

	context->pollEvents();
	
	// refresh fps display
	{
		float fps = 1.0f / static_cast<float>( dt );
		
		if ( fps > m_maxFps )
			m_maxFps = fps;

		m_fpsCache[ m_fpsCacheCounter ] = fps;
		m_fpsCacheCounter++;
		if ( m_fpsCacheCounter > FPS_CACHE_NUM )
		{
			m_fpsCacheCounter = 0;
			for ( int i = 0; i < FPS_CACHE_NUM; i++ )
				m_averageFps += m_fpsCache[ i ];
			m_averageFps /= (float)FPS_CACHE_NUM;

		}

		std::string title = "FPS: " + std::to_string( (int)m_averageFps ) + "   MAX: " + std::to_string( (int)m_maxFps );
		context->setTitle( title.c_str() );
	}

	currentCamera->update( dt );

	/// ------------------ render ------------------ ///
	
	
	if ( !m_gbuffer || m_defaultRenderTarget->width == 0 || m_defaultRenderTarget->height == 0 )
		return;

	device->setRenderTarget( m_gbuffer );
	device->clearRenderTarget( true, true );

	device->draw( m_monke );

	/// TODO: remove raw gl calls
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	device->draw( m_skybox );
	glDepthFunc( GL_LESS );
	glDepthMask( GL_TRUE );

	// normal back buffer
	device->setRenderTarget( m_defaultRenderTarget );
	device->clearRenderTarget( true, true );

	// bind gbuffer textures to deferred pass
	for ( int i = 0; i < m_gbuffer->numTextures; i++ )
		device->bindTextureToSlot( m_gbuffer->textures[ i ], i );

	// render screen quad with deferred shader
	device->setActivePipeline( m_deferredPipeline );
	device->draw( m_screenQuad );

	context->swapBuffers();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::createScreeQuad()
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
	m_screenQuad = device->createMesh( &meshDesc );
	device->createPrimitive( &prDesc, m_screenQuad );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::createGBuffer()
{
	RenderTargetDesc rtDesc;
	rtDesc.width = context->getWidth();
	rtDesc.height = context->getHeight();

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

	m_gbuffer = device->createRenderTarget( &rtDesc );
}
