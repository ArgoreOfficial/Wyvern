#include "Application.h"

#include <stdio.h>

#include <wv/Device/GraphicsDevice.h>
#include <glad/glad.h>
#include <wv/Device/Context.h>


#include <wv/Assets/Texture.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <math.h>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

#include <wv/Memory/MemoryDevice.h>

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

	device = wv::GraphicsDevice::createGraphicsDevice( &deviceDesc );

	m_defaultRenderTarget = new RenderTarget();
	m_defaultRenderTarget->width = _desc->windowWidth;
	m_defaultRenderTarget->height = _desc->windowHeight;
	m_defaultRenderTarget->fbHandle = 0;
	
	s_instance = this;

	createDeferredPipeline();
	createScreeQuad();
	createGBuffer();

	device->setRenderTarget( m_defaultRenderTarget );

	memoryDevice = new MemoryDevice();
}

wv::Application* wv::Application::getApplication()
{
	return s_instance;
}

#ifdef EMSCRIPTEN
void emscriptenMainLoop() { wv::Application::getApplication()->tick(); }
#endif

/// TEMPORARY---
// called the first time device->draw() is called that frame
void pipelineCB( wv::UniformBlockMap& _uniformBlocks )
{
	wv::Application* app = wv::Application::getApplication();
	wv::Context* ctx = app->context;

	// material properties
	wv::UniformBlock& fragBlock = _uniformBlocks[ "UbInput" ];
	const wv::float3 psqYellow = { 0.9921568627f, 0.8156862745f, 0.03921568627f };

	fragBlock.set<wv::float3>( "u_Color", psqYellow );
	fragBlock.set<float>( "u_Alpha", 1.0f );

	// camera transorm
	wv::UniformBlock& block = _uniformBlocks[ "UbInstanceData" ];

	glm::mat4x4 projection = app->currentCamera->getProjectionMatrix();
	glm::mat4x4 view = app->currentCamera->getViewMatrix();
	
	block.set( "u_Projection", projection );
	block.set( "u_View", view );

	// bind texture to slot 0
	app->device->bindTextureToSlot( app->m_texture, 0 );
}

// called every time device->draw() is called 
void instanceCB( wv::UniformBlockMap& _uniformBlocks )
{
	wv::Application* app = wv::Application::getApplication();
	wv::Context* ctx = app->context;

	// model transform
	wv::UniformBlock& instanceBlock = _uniformBlocks[ "UbInstanceData" ];

	glm::mat4x4 model{ 1.0f };
	instanceBlock.set( "u_Model", model );
}
/// ---TEMPORARY


void wv::Application::run()
{
	/// TODO: not this
	{
		wv::ShaderSource shaders[] = {
			{ wv::WV_SHADER_TYPE_VERTEX,   "res/vert.glsl" },
			{ wv::WV_SHADER_TYPE_FRAGMENT, "res/frag.glsl" }
		};

		/// TODO: change to UniformDesc?
		const char* ubInputUniforms[] = {
			"u_Color",
			"u_Alpha"
		};

		const char* ubInstanceDataUniforms[] = {
			"u_Projection",
			"u_View",
			"u_Model",
		};

		wv::UniformBlockDesc uniformBlocks[] = {
			{ "UbInput",        ubInputUniforms,        2 },
			{ "UbInstanceData", ubInstanceDataUniforms, 3 }
		};

		wv::PipelineDesc pipelineDesc;
		pipelineDesc.type = wv::WV_PIPELINE_GRAPHICS;
		pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
		pipelineDesc.shaders = shaders;
		pipelineDesc.uniformBlocks = uniformBlocks;
		pipelineDesc.numUniformBlocks = 2;
		pipelineDesc.numShaders = 2;
		pipelineDesc.instanceCallback = instanceCB;
		pipelineDesc.pipelineCallback = pipelineCB;

		m_pipeline = device->createPipeline( &pipelineDesc );
	}


	{
		wv::InputLayoutElement elements[] = {
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 },
			{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }
		};

		wv::InputLayout layout;
		layout.elements = elements;
		layout.numElements = 2;

		// custom mesh export
		/*
		// data
		int pnumIndices = 0;
		int pnumVertices = 36;

		std::ofstream cubefile( "res/cube.wpr" );
		// header
		cubefile.write( (char*)&pnumIndices, sizeof( int ) );
		cubefile.write( (char*)&pnumVertices, sizeof( int ) );
		// data
		cubefile.write( (char*)indices, sizeof( indices ) );
		cubefile.write( (char*)skyboxVertices, sizeof( skyboxVertices ) );
		cubefile.close();
		*/
		
		/// TODO: change to proper asset loader
		std::ifstream in( "res/cube.wpr", std::ios::binary );
		std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

		wv::PrimitiveDesc prDesc;
		{
			int numIndices  = *reinterpret_cast<int*>( buf.data() );
			int numVertices = *reinterpret_cast<int*>( buf.data() + sizeof( int ) );
			int vertsSize = numVertices * sizeof( float ) * 5; // 5 floats per vertex
			int indsSize  = numIndices * sizeof( unsigned int );

			char* indexBuffer  = buf.data() + ( sizeof( int ) * 2 );
			char* vertexBuffer = indexBuffer + indsSize;

			prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
			prDesc.layout = &layout;

			prDesc.vertexBuffer = vertexBuffer;
			prDesc.vertexBufferSize = vertsSize;
			prDesc.numVertices = numVertices; /// TODO: don't hardcode

			prDesc.indexBuffer = 0;
			prDesc.indexBufferSize = 0;
			prDesc.numIndices = 0;
		}

		m_primitive = device->createPrimitive( &prDesc );
	}

	// Subscribe to mouse event
	subscribeMouseEvents();
	subscribeInputEvent();

	// cameras have to be made after the event is subscribed to
	// to get the correct order
	/// TODO: event priority
	orbitCamera = new OrbitCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	freeflightCamera = new FreeflightCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	orbitCamera->onCreate();
	freeflightCamera->onCreate();

	currentCamera = orbitCamera;

	TextureMemory texMem = memoryDevice->loadTextureData( "res/throbber.gif" );
	TextureDesc texDesc;
	texDesc.memory = &texMem;
	m_texture = device->createTexture( &texDesc );
	memoryDevice->unloadTextureData( &texMem );
	
#ifdef EMSCRIPTEN
	emscripten_set_main_loop( &emscriptenMainLoop, 0, 1 );
#else
	while ( context->isAlive() )
		tick();
#endif
}

void wv::Application::terminate()
{
	currentCamera = nullptr;
	
	delete orbitCamera;
	delete freeflightCamera;
	orbitCamera = nullptr;
	freeflightCamera = nullptr;

	device->destroyPrimitive( &m_primitive );
	device->destroyPipeline( &m_pipeline );
	device->destroyTexture( &m_texture );

	device->destroyPrimitive( &m_screenQuad );
	device->destroyPipeline( &m_deferredPipeline );
	device->destroyRenderTarget( &m_gbuffer );

	context->terminate();
	device->terminate();

	delete context;
	delete device;
	delete memoryDevice;
}

void wv::Application::tick()
{
	double dt = context->getDeltaTime();
	
	/// ------------------ update ------------------ 

	context->pollEvents();
	
	std::string fps = "FPS: " + std::to_string(( 1.0 / dt ));
	context->setTitle( fps.c_str() );

	currentCamera->update( dt );

	/// ------------------ render ------------------ 

	device->setRenderTarget( m_gbuffer );

	// cube
	device->setActivePipeline( m_pipeline );
	device->clearRenderTarget( wv::Colors::White );
	device->draw( m_primitive );
	
	// normal back buffer
	device->setRenderTarget( m_defaultRenderTarget );
	
	// bind gbuffer textures to deferred pass
	for ( int i = 0; i < m_gbuffer->numTextures; i++ )
		device->bindTextureToSlot( m_gbuffer->textures[ i ], i );

	// render screen quad with deferred shader
	device->setActivePipeline( m_deferredPipeline );
	device->draw( m_screenQuad );

	context->swapBuffers();
}

void wv::Application::onResize( int _width, int _height )
{
	context->onResize( _width, _height );
	device->onResize( _width, _height );

	// recreate render target
	m_defaultRenderTarget->width = _width;
	m_defaultRenderTarget->height = _height;
	device->setRenderTarget( m_defaultRenderTarget );
}

void wv::Application::onMouseEvent( MouseEvent _event )
{
	if ( _event.button != MouseEvent::WV_MOUSE_BUTTON_RIGHT )
		return;

	if ( !_event.buttondown )
		return;

	if( currentCamera == orbitCamera )
	{
		currentCamera = freeflightCamera;
		freeflightCamera->getTransform().position = orbitCamera->getTransform().position;
		freeflightCamera->getTransform().rotation = orbitCamera->getTransform().rotation;

		( (FreeflightCamera*)freeflightCamera )->resetVelocity();
	}
}

void wv::Application::onInputEvent( InputEvent _event )
{
	if ( !_event.repeat )
		if ( _event.key == GLFW_KEY_F )
			if ( currentCamera != orbitCamera )
				currentCamera = orbitCamera; // lol
}

void wv::Application::createDeferredPipeline()
{
	wv::ShaderSource shaders[] = {
		{ wv::WV_SHADER_TYPE_VERTEX,   "res/deferred_vs.glsl" },
		{ wv::WV_SHADER_TYPE_FRAGMENT, "res/deferred_fs.glsl" }
	};

	wv::PipelineDesc pipelineDesc;
	pipelineDesc.type     = wv::WV_PIPELINE_GRAPHICS;
	pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
	pipelineDesc.shaders    = shaders;
	pipelineDesc.numShaders = 2;
	
	m_deferredPipeline = device->createPipeline( &pipelineDesc );

	unsigned int texALoc = glGetUniformLocation( m_deferredPipeline->program, "u_TextureA" );
	unsigned int texBLoc = glGetUniformLocation( m_deferredPipeline->program, "u_TextureB" );

	glUseProgram( m_deferredPipeline->program );
	glUniform1i( texALoc, 0 );
	glUniform1i( texBLoc, 1 );
	glUseProgram( 0 );
}

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
		-1.0f, -1.0f, 0.5f,       0.0f, 0.0f,
		-1.0f,  3.0f, 0.5f,       0.0f, 2.0f,
		 3.0f, -1.0f, 0.5f,       2.0f, 0.0f
	};

	wv::PrimitiveDesc prDesc;
	{
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;

		prDesc.vertexBuffer     = vertices;
		prDesc.vertexBufferSize = sizeof( vertices );
		prDesc.numVertices      = 3;
	}

	m_screenQuad = device->createPrimitive( &prDesc );
}

void wv::Application::createGBuffer()
{
	RenderTargetDesc rtDesc;
	rtDesc.width = context->getWidth();
	rtDesc.height = context->getHeight();

	TextureDesc texDescs[] = {
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_BYTE },
		{ wv::WV_TEXTURE_CHANNELS_RGBA, wv::WV_TEXTURE_FORMAT_BYTE }
	};
	rtDesc.textureDescs = texDescs;
	rtDesc.numTextures = 2;

	m_gbuffer = device->createRenderTarget( &rtDesc );
}
